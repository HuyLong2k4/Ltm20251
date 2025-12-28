#ifndef FUNCTION_H
#define FUNCTION_H

#include <mysql/mysql.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "queryUser.h"

void handleRequest(MYSQL *conn, char *type, int connfd, char **username, char *password, listLoginedAccount *arr, node h);

void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password);

void handleRegister(MYSQL *conn, int connfd, node *h);

void handleChangePassword(int connfd, MYSQL *conn, node* h);

void handleLogout(int connfd, listLoginedAccount *arr, char *username);



void handleSearchFilmByTitle(MYSQL *conn, int connfd, char *title);

void handleShowFilm(MYSQL *conn, int connfd);
void handleShowCinemaByFilm(MYSQL *conn, int connfd);
void handleShowTimeByFilmCinema(MYSQL *conn, int connfd);
void handleShowSeat(MYSQL *conn, int connfd);
void handleBookTicket(MYSQL *conn, int connfd, char *username, char *showtime_id, char *seat_id);

void handleShowCategory(MYSQL *conn, int connfd);
void handleShowCinema(MYSQL *conn, int connfd);
void handleShowPremieredTime(MYSQL *conn, int connfd);

void handleBrowseCategory(MYSQL *conn, int connfd, char *category_id);
void handleBrowseCinema(MYSQL *conn, int connfd, char *cinema_id);
void handleBrowseShowTime(MYSQL *conn, int connfd, char *premiered_time_id);



#endif