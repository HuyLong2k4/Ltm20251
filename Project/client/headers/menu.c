#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"

void viewWelcome(){
    printf("--------------Welcome------------------\n\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n\n");
    printf("---------------------------------------------------\n");
}

void viewLogin(char username[], char password[]){
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);
}

void viewRegister(char *name, char *username, char *password) {
    printf("Registration\n");
    printf("Full Name: ");
    memset(name, 0, 255);
    fgets(name, 255, stdin);
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);
}

void viewUser(){
    printf("--------------Welcome to HTV-SPM------------------\n\n");
    printf("1. Browse film catalogs\n");
    printf("2. Search film \n");
    printf("3. Book ticket\n");
    printf("4. Logout\n");
    printf("5. Change Password\n");
    printf("---------------------------------------------------\n");
}

void viewChangePassword(char *oldPassword, char *newPassword) {
    printf("Change Password\n");
    printf("Old Password: ");
    scanf("%s", oldPassword);
    printf("New Password: ");
    scanf("%s", newPassword);
}


// --------------------

void getInput(const char *prompt, char *out, int max_len) {
    printf("%s", prompt);
    fgets(out, max_len, stdin);
    out[strcspn(out, "\n")] = 0; // bỏ '\n'
}

/*----------ADD FILM--------*/
// void addNewFilm(int sockfd, char title[], char category_id[], char show_time[], char description[]){
//     printf("Title: ");
//     title[0] = '\0';
//     fgets(title, 255, stdin);

//     char *message = (char *)malloc(255 * sizeof(char));
//     makeShowCategoryMessage(message);
//     sendMessage(sockfd, message);
//     recvMessage(sockfd, message);

//     printf("Category: \n");
//     printf("%s\n", message);
//     printf("Choice: ");
//     category_id[0] = '\0';
//     fgets(category_id, 255, stdin);


//     printf("Show time: ");
//     show_time[0] = '\0';
//     fgets(show_time, 255, stdin);    

//     printf("Description: ");
//     description[0] = '\0';
//     fgets(description, 2048, stdin);

// }

void addNewFilm(int sockfd, char title[], char category_id[], char show_time[], char description[]) {

    getInput("Title: ", title, 255);

    char message[1024];  

    makeShowCategoryMessage(message);
    sendMessage(sockfd, message);
    recvMessage(sockfd, message);

    printf("Category:\n%s\n", message);
    getInput("Choice: ", category_id, 255);

    getInput("Show time: ", show_time, 255);
    getInput("Description: ", description, 2048);
}

/*----------END ADD FILM--------*/

/*----------FIND FILM--------*/
void getTitleFilm(char *title) {
    getInput("Title: ", title, 255);
}
/*----------END FIND FILM--------*/

/*----------BROWSE FILM--------*/
void browseFilm(){
    printf("--------------BROWSE FILM------------------\n\n");
    printf("1. By category\n");
    printf("2. By cinema\n");
    printf("3. Go back\n");
    printf("---------------------------------------------------\n\n");
}

void getCategoryID(char *category_id) {
    getInput("Enter category ID: ", category_id, 255);
}

void getPremieredTimeID(char *time_id) {
    getInput("Enter premiered time ID: ", time_id, 255);
}

/*----------END BROWSE FILM--------*/


/*----------BOOK TICKET--------*/
void bookTicket(){
    printf("--------------Welcome to HTV-SPM------------------\n\n");
    printf("1. Book Ticket\n");
    printf("2. Return\n\n");
    printf("---------------------------------------------------\n");
}

void getFilmID(char *film_id) {
    getInput("Enter film ID: ", film_id, 255);
}

void getCinemaID(char *cinema_id) {
    getInput("Enter cinema ID: ", cinema_id, 255);
}

void getShowTimeID(char *showtime_id) {
    getInput("Enter showtime ID: ", showtime_id, 255);
}

void getSeatID(char *seat_id) {
    getInput("Enter seat ID: ", seat_id, 255);
}

/*----------END BOOK TICKET--------*/

void viewManager(){
    printf("--------------Welcome to Manager-----------------\n\n");
    printf("1. Add new film\n");
    printf("2. Announcing film \n");
    printf("3. Edit announced film\n");
    printf("4. Logout\n\n");
    printf("---------------------------------------------------\n");
}
