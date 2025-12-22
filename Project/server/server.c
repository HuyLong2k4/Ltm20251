#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "../lib/socket/socket.h"
#include "../lib/messages/message.h"
#include "headers/function.h"
#include "headers/queryUser.h"
#include "../db/connect.h"

#define BACKLOG 20

// Đảm bảo các define này khớp với menu.h ở Client
#define LOGIN_SUCCESS_USER 1010
#define LOGIN_SUCCESS_ADMIN 1011
#define LOGIN_FAIL 2011
#define REGISTER_SUCCESS 1020
#define REGISTER_FAIL 2021
#define LOGOUT_SUCCESS 1030

// --- KHAI BÁO MUTEX ĐỂ KHÓA DATABASE ---
pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;
MYSQL *conn;
listLoginedAccount arr;
node head = NULL;

void *handleCommunicate(void *);

int main(int argc, char **argv){
    if(argc != 2){
        printf("Usage: ./server_out <number port>\n");
        exit(1);
    }

    // Ket noi database
    connectDatabase(&conn);
    if(conn == NULL){
        printf("Cannot connect to database\n");
        exit(1);
    }
    printf("Connected to database successfully\n");

    // Initialize the logged-in accounts list and user list
    arr = createListLoginedAccount();
    selectUser(conn, &head, (user){0});

    int listenfd, *connfd;
    struct sockaddr_in server;
    struct sockaddr_in *client;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    pthread_t tid;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("\nError: ");
        return 0;
    }

    // có thể tắt bật server thoải mái mà không bị lỗi "Address already in use".
    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // ---------------------------------------------

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("\nError: ");
        return 0;
    }

    if (listen(listenfd, BACKLOG) == -1) {
        perror("\nError: ");
        return 0;
    }

    printf("Server is listening on port %s...\n", argv[1]);
    client = malloc(sin_size);

    while (1) {
        connfd = malloc(sizeof(int));
        *connfd = accept(listenfd, (struct sockaddr*) client, &sin_size);

        if (*connfd == -1) {
            perror("\nError: ");
            free(connfd);
            continue;
        }

        // Tạo luồng mới
        pthread_create(&tid, NULL, &handleCommunicate, connfd);
    }

    close(listenfd);
    return 0;
}

void *handleCommunicate(void* arg){
    int connfd = *((int*) arg);
    free(arg);
    pthread_detach(pthread_self());
    
    if(connfd == -1){
        return NULL;
    }
    
    printf("Client connected (fd: %d)\n", connfd);
    send(connfd, "Connected to server successfully!", 255, 0);

    char message[255];
    char message_copy[255];

    while(1){
        memset(message, 0, 255);
        
        // Giả sử hàm recvMessage trả về số byte nhận được hoặc <=0 nếu ngắt kết nối
        // Cần kiểm tra kỹ hàm recvMessage của bạn
        recvMessage(connfd, message); 
        
        // Nếu message rỗng hoặc client đóng kết nối
        if (strlen(message) == 0) {
            printf("Client (fd: %d) disconnected unexpected.\n", connfd);
            break;
        }

        if(strcmp(message, "EXIT") == 0){
            printf("Client (fd: %d) disconnected safely.\n", connfd);
            break;
        }
        
        strcpy(message_copy, message);
        
        char *type = getTypeMessage(message_copy);
        
        if(type == NULL) {
            continue;
        }

        printf("Request type from fd %d: %s\n", connfd, type);
        
        if(strcmp(type, "LOGIN") == 0){
            char username[255] = {0}, password[255] = {0};
            getLoginMessage(username, password);
            
            handleLogin(connfd, &arr, &head, username, password);
            
        } else if(strcmp(type, "REGISTER") == 0){
            char name[255] = {0}, username[255] = {0}, password[255] = {0};
            getRegisterMessage(name, username, password);
            
            handleRegister(conn, connfd, &head);
            
        } else if(strcmp(type, "LOGOUT") == 0){
            sendResult(connfd, LOGOUT_SUCCESS);
            
        } else if(strcmp(type, "CHANGE_PASSWORD") == 0){
            handleChangePassword(connfd, conn, &head);
            
        } else {
            sendResult(connfd, LOGIN_FAIL); 
        }
    }

    close(connfd);
    return NULL;
}
