#ifndef MENU_H
#define MENU_H

#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"

/*====================RESULT CODE====================*/

/*----------ACCOUNT----------*/
#define LOGIN_SUCCESS_USER 1010
#define LOGIN_SUCCESS_MANAGER 1011
#define LOGIN_SUCCESS_ADMIN 1012
#define LOGIN_FAIL 2011
#define LOGIN_ALREADY 2012
#define LOGOUT_SUCCESS 1030
#define REGISTER_SUCCESS 1020
#define REGISTER_FAIL 2021
#define CHANGE_PASSWORD_SUCCESS 1110
#define CHANGE_PASSWORD_FAIL 2110

#define LOGIN_SUCCESS_MESSAGE "You have logged in successfully!!\n"
#define LOGIN_FAIL_MESSAGE "Your username or password is incorrect!!\n"
#define LOGIN_ALREADY_MESSAGE "Your account is being used in another address!!\n"
#define REGISTER_SUCCESS_MESSAGE "You have registered successfully!!\n"
#define REGISTER_FAIL_MESSAGE "Username has been existed!!\n"
#define LOGOUT_SUCCESS_MESSAGE "You have been successfully logged out!!\n"
#define CHANGE_PASSWORD_SUCCESS_MESSAGE "Changed password successfully\n"
#define CHANGE_PASSWORD_FAIL_MESSAGE "Changed password fail!!\n" 
/*----------END ACCOUNT----------*/

/*----------FIND FILM--------*/
#define FIND_FILM_SUCCESS 1073
#define FIND_FILM_SUCCESS_MESSAGE "The information of the film you need search:\n"
#define FIND_FILM_FAIL 2072 
#define FIND_FILM_FAIL_MESSAGE "No film has been found!!\n"
/*----------FIND FILM--------*/

/*----------BROWSE FILM--------*/
#define BROWSE_CATEGORY_SUCCESS 1070
#define BROWSE_CATEGORY_SUCCESS_MESSAGE "Browse follow category successfully!!\n"
#define BROWSE_THEATER_SUCCESS 1071
#define BROWSE_THEATER_SUCCESS_MESSAGE "Browse follow theater successfully!!\n"
#define BROWSE_TIME_SUCCESS 1072
#define BROWSE_TIME_SUCCESS_MESSAGE "Browse follow time"
#define BROWSE_FAIL 2071
#define BROWSE_FAIL_MESSAGE "No film has been found!!\n"
/*----------END BROWSE FILM--------*/

/*----------BOOK TICKET--------*/
#define BOOK_TICKET_SUCCESS 1200
#define BOOK_TICKET_SUCCESS_MESSAGE "Booking ticket successfully!\n"
#define BOOK_TICKET_FAIL 2200
#define BOOK_TICKET_FAIL_MESSAGE "Booking ticket failed! Seat may be occupied.\n"
/*----------END BOOK TICKET--------*/

/*----------ADD FILM--------*/
#define ADD_FILM_SUCCESS 1040 
#define ADD_FILM_SUCCESS_MESSAGE "Add new film successfully!!\n"
#define ADD_FILM_FAIL 2041
#define ADD_FILM_FAIL_MESSAGE "ADD new film failed!!\n"
#define FILM_EXISTS 2042
#define FILM_EXISTS_MESSAGE "Film already exists in the system!!\n"
/*----------END ADD FILM--------*/

/*----------ADMIN MANAGEMENT----------*/
#define SHOW_ALL_USERS_SUCCESS 1300
#define SHOW_ALL_USERS_FAIL 2300
#define DELETE_USER_SUCCESS 1301
#define DELETE_USER_FAIL 2301
#define UPDATE_USER_ROLE_SUCCESS 1302
#define UPDATE_USER_ROLE_FAIL 2302

#define SHOW_ALL_USERS_SUCCESS_MESSAGE "User list retrieved successfully!\n"
#define SHOW_ALL_USERS_FAIL_MESSAGE "Failed to retrieve user list!\n"
#define DELETE_USER_SUCCESS_MESSAGE "User deleted successfully!\n"
#define DELETE_USER_FAIL_MESSAGE "Failed to delete user!\n"
#define UPDATE_USER_ROLE_SUCCESS_MESSAGE "User role updated successfully!\n"
#define UPDATE_USER_ROLE_FAIL_MESSAGE "Failed to update user role!\n"
/*----------END ADMIN MANAGEMENT----------*/

/*====================END RESULT CODE====================*/

void viewWelcome();
void viewLogin(char username[], char password[]);
void viewRegister(char *name, char *username, char *password);
void viewChangePassword(char *oldPassword, char *newPassword);
void viewUser();

// void viewChangePassword(char *oldPassword, char *newPassword);

/*----------ADD FILM--------*/
void addNewFilm(int sockfd, char title[], char category_id[], char show_time[], char description[]);

/*----------END ADD FILM--------*/

/*----------FIND FILM--------*/
void getTitleFilm(char title[]);
/*----------END FIND FILM--------*/

/*----------BROWSE FILM--------*/
void browseFilm();

// void getCategoryID(char **category_id);
// void getCinemaID(char **cinema_id);

void getCategoryID(char *category_id);
void getPremieredTimeID(char *time_id);

/*----------END BROWSE FILM--------*/

void getCinemaID(char *cinema_id);

/*----------BOOK TICKET--------*/
void bookTicket();

void getInput(const char *prompt, char *out, int max_len);

void getFilmID(char *film_id);



void getShowTimeID(char *showtime_id);

void getSeatID(char *seat_id);

/*----------END BOOK TICKET--------*/


void viewChangePassword(char *oldPassword, char *newPassword);
void viewManager();
void viewAdmin();
#endif