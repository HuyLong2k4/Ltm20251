#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "queryUser.h"

#define LOGIN_SUCCESS_USER     1010
#define LOGIN_SUCCESS_ADMIN    1011
#define LOGIN_FAIL             2011
#define LOGIN_ALREADY          2012
#define LOGOUT_SUCCESS         1030
#define REGISTER_SUCCESS       1020
#define REGISTER_FAIL          2021
#define CHANGE_PASSWORD_SUCCESS 1110
#define CHANGE_PASSWORD_FAIL    2110

void handleRequest( MYSQL *conn, char *type, int connfd, char *username, char *password, listLoginedAccount *arr, node *h);
void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password);
void handleLogout( int connfd, listLoginedAccount *arr, char *username);
void handleRegister( MYSQL *conn, int connfd, node *h);
void handleChangePassword( int connfd, MYSQL *conn, node *h);

void handleRequest( MYSQL *conn, char *type, int connfd, char *username, char *password, listLoginedAccount *arr, node *h){
    if (strcmp(type, "LOGIN") == 0) {
        handleLogin(connfd, arr, h, username, password);
    }
    else if (strcmp(type, "LOGOUT") == 0) {
        handleLogout(connfd, arr, username);
    }
    else if (strcmp(type, "REGISTER") == 0) {
        handleRegister(conn, connfd, h);
    }
    else if (strcmp(type, "CHANGE_PASSWORD") == 0) {
        handleChangePassword(connfd, conn, h);
    }
}

void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password){
    int check = checkLogin(*h, &username, password, arr);
    printf("Login check result: %d\n", check);

    if (check == 0) {
        sendResult(connfd, LOGIN_FAIL);
    }
    else if (check == 1) {
        addToListLoginedAccount(arr, &username);
        sendResult(connfd, LOGIN_SUCCESS_ADMIN);
    }
    else if (check == 2) {
        addToListLoginedAccount(arr, &username);
        sendResult(connfd, LOGIN_SUCCESS_USER);
    }
    else {
        sendResult(connfd, LOGIN_ALREADY);
    }
}

void handleLogout(int connfd, listLoginedAccount *arr, char *username){
    printf("Logout user: %s\n", username);
    deleteFromListLoginedAccount(arr, &username);
    sendResult(connfd, LOGOUT_SUCCESS);
}

void handleRegister(MYSQL *conn, int connfd, node *h){
    char name[255] = {0};
    char username[255] = {0};
    char password[255] = {0};

    getRegisterMessage(name, username, password);

    user newUser;
    strncpy(newUser.name, name, sizeof(newUser.name) - 1);
    strncpy(newUser.username, username, sizeof(newUser.username) - 1);
    strncpy(newUser.password, password, sizeof(newUser.password) - 1);
    newUser.role_id = 2;

    int result = registerUser(conn, newUser);

    if (result == 1) {
        addNode(h, newUser);
        sendResult(connfd, REGISTER_SUCCESS);
    } else {
        sendResult(connfd, REGISTER_FAIL);
    }
}

void handleChangePassword( int connfd, MYSQL *conn, node *h){
    char username[255] = {0};
    char oldPassword[255] = {0};
    char newPassword[255] = {0};

    getChangePasswordMessage(username, oldPassword, newPassword);

    int result = changePassword(conn, username, oldPassword, newPassword);
    printf("Change password result: %d\n", result);

    if (result == 1) {
        changeNodePassword(h, username, newPassword);
        sendResult(connfd, CHANGE_PASSWORD_SUCCESS);
    } else {
        sendResult(connfd, CHANGE_PASSWORD_FAIL);
    }
}
