#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "menu.h"

void clearKeyboardBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {

    }
}

void handleLogout(int sockfd, char *message);
void handleRegister(int sockfd, char *message);
void handleChangePassword(int sockfd, char *username, char *message);



void handleLogin(int sockfd, char *username, char *password, char *message, int *login_status){
    viewLogin(username, password);
    makeLoginMessage(username, password, message);
    sendMessage(sockfd, message);
    *login_status = recvResult(sockfd);
    if(*login_status == LOGIN_FAIL){
        printf("%s\n", LOGIN_FAIL_MESSAGE);
    }else if(*login_status == LOGIN_ALREADY){
        printf("%s\n", LOGIN_ALREADY_MESSAGE);
    }else{
        printf("%s\n", LOGIN_SUCCESS_MESSAGE);
    }
}

void handleRegister(int sockfd, char *message){
    char newName[255], newUsername[255], newPassword[255];

    viewRegister(newName, newUsername, newPassword);
    makeRegisterMessage(newName, newUsername, newPassword, message);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if (result == REGISTER_SUCCESS) {
        printf(REGISTER_SUCCESS_MESSAGE);
    } else {
        printf(REGISTER_FAIL_MESSAGE);
    }
}

void handleRequestUser(int sockfd, char *username, char *message) {
    int user_choice;
    do {
        viewUser();
        printf("Choice: ");
        scanf("%d", &user_choice);
        clearKeyboardBuffer();
        switch(user_choice) {
            case 1:{
                // handleBrowseFilm(sockfd);
                break;
            }
            case 2:{
                // char *title;
                // handleSearchByTitle(sockfd, title, message);
                break;
            }
            case 3:{
                //handleBookTicket();
                break;
            }
            case 4:{
                handleLogout(sockfd, message);
                user_choice = 0;
                break;
            }
            case 5:{
                printf("%s\n", username);
                handleChangePassword(sockfd, username, message);
                break;
            }
        }
    } while (user_choice != 0);
}

void handleLogout(int sockfd, char *message){
    makeLogoutMessage(message);
    sendMessage(sockfd, message);
    int result = recvResult(sockfd);
    printf("%d\n", result);
    printf(LOGOUT_SUCCESS_MESSAGE);
}

void handleChangePassword(int sockfd, char *username, char *message){
    char oldPassword[255], newPassword[255];
    viewChangePassword(oldPassword, newPassword);
    makeChangePasswordMessage(username, oldPassword, newPassword, message);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    printf("%d\n", result);
    if (result == CHANGE_PASSWORD_SUCCESS) {
        printf(CHANGE_PASSWORD_SUCCESS_MESSAGE);
    } else {
        printf(CHANGE_PASSWORD_FAIL_MESSAGE);
    }
}
