#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "queryUser.h"
#include "queryPremieredTime.h"
#include "queryPremieredTimeFilm.h"
#include "queryFilm.h"
#include "queryCategory.h"
#include "function.h"
#include "queryCinema.h"
#include "queryPremieredTime.h"
#include "queryPremieredTimeFilm.h"


#define LOGIN_SUCCESS_USER 1010
#define LOGIN_SUCCESS_ADMIN 1011
#define LOGIN_FAIL 2011
#define LOGIN_ALREADY 2012
#define LOGOUT_SUCCESS 1030
#define REGISTER_SUCCESS 1020
#define REGISTER_FAIL 2021
#define ADD_FILM_SUCCESS 1040
#define ADD_FILM_FAIL 2041
#define POST_FILM_SUCCESS 1050
#define POST_FILM_FAIL 2051
#define EDIT_FILM_SUCCESS 1060
#define EDIT_FILM_FAIL 2061
#define NO_EDIT_FILM 2062
#define BROWSE_CATEGORY_SUCCESS 1070
#define BROWSE_THEATER_SUCCESS 1071
#define BROWSE_TIME_SUCCESS 1072
#define BROWSE_FAIL 2071
#define FIND_FILM_SUCCESS 1073
#define FIND_FILM_FAIL 2072
#define VIEW_CHAIR_SUCCESS 1080
#define VIEW_CHAIR_FAIL 2081
#define CHOOSE_CHAIR_SUCCESS 1090
#define CHOOSE_CHAIR_FAIL 2091
#define BOOK_TICKET_SUCCESS 1100
#define BOOK_TICKET_FAIL 2101
#define CHANGE_PASSWORD_SUCCESS 1110
#define CHANGE_PASSWORD_FAIL 2110

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

// premiered: công chiếu
// void handleAnnouncingFilm( int connfd, MYSQL *conn, nodeFilm f, nodeCinema ci, nodePremieredTime pt, nodePremieredTimeFilm ptf ){
//     char film_id[255] = {0};
//     char cinema_id[255] = {0};
//     char premiered_time_id[255] = {0}; 
//     char date[255] = {0};
//     getAnnounceFilmMessage(film_id, cinema_id, premiered_time_id, date);
//     // Ý nghĩa: ID gửi từ client là string, Cần convert sang số để, tìm kiếm, lưu DB
//     unsigned long film_id_search = strtoul(film_id, NULL, 10);
//     unsigned long cinema_id_search = strtoul(cinema_id, NULL, 10);
//     unsigned long premiered_time_search = strtoul(premiered_time_id, NULL, 10);

//     int seru = searchPremieredTimeFilmToPost(ptf, film_id_search, cinema_id_search, premiered_time_search, date);
//     if (seru != 0)
//     {
//         sendResult(connfd, POST_FILM_FAIL);
//     }
//     else
//     {
//         premieredTimeFilm newPremieredTimeFilm;

//         newPremieredTimeFilm.film_id = film_id_search;
//         newPremieredTimeFilm.cinema_id = cinema_id_search;
//         newPremieredTimeFilm.premiered_time_id = premiered_time_search;
//         strcpy(newPremieredTimeFilm.date, date);

//         addNodePremieredTimeFilm(&ptf, newPremieredTimeFilm);

//         addPremieredTimeFilm(conn, newPremieredTimeFilm);

//         sendResult(connfd, POST_FILM_SUCCESS);
//     }
// }
