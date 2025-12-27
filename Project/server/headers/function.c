#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "queryUser.h"
#include <pthread.h>

// --- Extern mutex từ server_out.c ---
extern pthread_mutex_t db_lock;


#define LOGIN_SUCCESS_USER     1010
#define LOGIN_SUCCESS_ADMIN    1011
#define LOGIN_FAIL             2011
#define LOGIN_ALREADY          2012
#define LOGOUT_SUCCESS         1030
#define REGISTER_SUCCESS       1020
#define REGISTER_FAIL          2021
#define CHANGE_PASSWORD_SUCCESS 1110
#define CHANGE_PASSWORD_FAIL    2110

/*----------FIND FILM--------*/
#define FIND_FILM_SUCCESS 1073
#define FIND_FILM_FAIL 2072 
/*----------END FIND FILM--------*/

/*----------BROWSE FILM----------*/
#define BROWSE_CATEGORY_SUCCESS 1070
#define BROWSE_THEATER_SUCCESS 1071
#define BROWSE_TIME_SUCCESS 1072
#define BROWSE_FAIL 2071
/*----------END BROWSE FILM----------*/

/*----------BOOKING TICKET----------*/
#define BOOK_TICKET_SUCCESS 1200
#define BOOK_TICKET_FAIL 2200
/*----------END BOOKING TICKET----------*/


// void handleRequest( MYSQL *conn, char *type, int connfd, char *username, char *password, listLoginedAccount *arr, node *h);
void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password);
void handleLogout( int connfd, listLoginedAccount *arr, char *username);
void handleRegister( MYSQL *conn, int connfd, node *h);
void handleChangePassword( int connfd, MYSQL *conn, node *h);



// void handleRequest( MYSQL *conn, char *type, int connfd, char *username, char *password, listLoginedAccount *arr, node *h){
//     if (strcmp(type, "LOGIN") == 0) {
//         handleLogin(connfd, arr, h, username, password);
//     }
//     else if (strcmp(type, "LOGOUT") == 0) {
//         handleLogout(connfd, arr, username);
//     }
//     else if (strcmp(type, "REGISTER") == 0) {
//         handleRegister(conn, connfd, h);
//     }
//     else if (strcmp(type, "CHANGE_PASSWORD") == 0) {
//         handleChangePassword(connfd, conn, h);
//     }
// }

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


/*----------FIND FILM--------*/
void handleSearchFilmByTitle(MYSQL *conn, int connfd, char *title) {
    char message[1024] = {0};
    char query[512];
    
    snprintf(query, sizeof(query),
        "SELECT f.id, f.title, c.name AS category, f.show_time "
        "FROM films f JOIN categories c ON f.category_id = c.id "
        "WHERE f.title LIKE '%%%s%%'", title);

    if (mysql_query(conn, query) != 0) {
        sendResult(connfd, FIND_FILM_FAIL);
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        sendResult(connfd, FIND_FILM_FAIL);
        if (res) mysql_free_result(res);
        return;
    }

    sendResult(connfd, FIND_FILM_SUCCESS);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        snprintf(message, sizeof(message),
            "ID: %s | Title: %s | Category: %s | Show_time: %s minutes",
            row[0], row[1], row[2], row[3]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

/*----------END FIND FILM--------*/

/*----------BROWSE FILM----------*/
/*----------BROWSE CATEGORY----------*/

void handleShowCategory(MYSQL *conn, int connfd){
    char query[512];
    char message[1024];

    sprintf(query, "SELECT id, name FROM categories");

    if(mysql_query(conn, query) != 0){
        sendMessage(connfd, "Cant load the list of categories!");
        sendMessage(connfd, "END");  
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendMessage(connfd, "No categories available!");
        sendMessage(connfd, "END"); 
        if(res) mysql_free_result(res);
        return;
    }

    // Gửi từng dòng, không ghép hết vào 1 message
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "%s. %s", row[0], row[1]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END"); 
    mysql_free_result(res);
}

void handleShowCinema(MYSQL *conn, int connfd){
    char query[512];
    char message[1024];

    sprintf(query, "SELECT id, name FROM cinemas");

    if(mysql_query(conn, query) != 0){
        sendMessage(connfd, "Cant load the list of cinemas!");
        sendMessage(connfd, "END"); 
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendMessage(connfd, "No cinemas available!");
        sendMessage(connfd, "END");  
        if(res) mysql_free_result(res);
        return;
    }


    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "%s. %s", row[0], row[1]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END"); 
    mysql_free_result(res);
}

void handleShowPremieredTime(MYSQL *conn, int connfd){
    char query[512];
    char message[1024];

    // Lấy các khung giờ chiếu
    sprintf(query, 
        "SELECT DISTINCT "
        "TIME_FORMAT(start_time, '%%H:%%i') as start, "
        "TIME_FORMAT(end_time, '%%H:%%i') as end "
        "FROM showtimes "
        "ORDER BY start_time");

    if(mysql_query(conn, query) != 0){
        sendMessage(connfd, "Cant load the list of showtimes!");
        sendMessage(connfd, "END");  
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendMessage(connfd, "No showtimes available!");
        sendMessage(connfd, "END");  
        if(res) mysql_free_result(res);
        return;
    }

   
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "%s - %s", row[0], row[1]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");  
    mysql_free_result(res);
}

void handleBrowseCategory(MYSQL *conn, int connfd, char *category_id){
    char query[512];
    char message[1024];

    sprintf(query, "SELECT f.id, f.title, c.name AS category, f.show_time "
                   "FROM films f JOIN categories c ON f.category_id = c.id "
                   "WHERE f.category_id = %s", category_id);

    if(mysql_query(conn, query) != 0){
        sendResult(connfd, BROWSE_FAIL);
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendResult(connfd, BROWSE_FAIL);
        if(res) mysql_free_result(res);
        return;
    }

    sendResult(connfd, BROWSE_CATEGORY_SUCCESS);

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "ID: %s | Title: %s | Category: %s | Show_time: %s minutes",
                 row[0], row[1], row[2], row[3]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}


void handleBrowseCinema(MYSQL *conn, int connfd, char *cinema_id){
    char query[512];
    char message[1024];

    sprintf(query, 
        "SELECT DISTINCT f.id, f.title, c.name AS category, f.show_time "
        "FROM films f "
        "JOIN categories c ON f.category_id = c.id "
        "JOIN showtimes st ON st.film_id = f.id "
        "JOIN rooms r ON st.room_id = r.id "
        "WHERE r.cinema_id = %s", 
        cinema_id);

    if(mysql_query(conn, query) != 0){
        sendResult(connfd, BROWSE_FAIL);
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendResult(connfd, BROWSE_FAIL);
        if(res) mysql_free_result(res);
        return;
    }

    sendResult(connfd, BROWSE_THEATER_SUCCESS);

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "ID: %s | Title: %s | Category: %s | Show_time: %s minutes",
                 row[0], row[1], row[2], row[3]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

void handleBrowseShowTime(MYSQL *conn, int connfd, char *time_slot){
    char query[512];
    char message[1024];

    // Tìm phim có suất chiếu trong khung giờ
    sprintf(query, 
        "SELECT DISTINCT f.id, f.title, c.name AS category, f.show_time "
        "FROM films f "
        "JOIN categories c ON f.category_id = c.id "
        "JOIN showtimes st ON st.film_id = f.id "
        "WHERE TIME_FORMAT(st.start_time, '%%H:%%i') = '%s'", 
        time_slot);

    if(mysql_query(conn, query) != 0){
        sendResult(connfd, BROWSE_FAIL);
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendResult(connfd, BROWSE_FAIL);
        if(res) mysql_free_result(res);
        return;
    }

    sendResult(connfd, BROWSE_TIME_SUCCESS);

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "ID: %s | Title: %s | Category: %s | Show_time: %s minutes",
                 row[0], row[1], row[2], row[3]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

/*----------END BROWSE FILM----------*/


/*----------BOOK TICKET--------*/
void handleShowFilm(MYSQL *conn, int connfd) {
    char query[512];
    char message[1024];

    sprintf(query, "SELECT id, title FROM films ORDER BY id");

    if(mysql_query(conn, query) != 0){
        sendMessage(connfd, "Cant load the list of films!");
        sendMessage(connfd, "END");
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendMessage(connfd, "No films available!");
        sendMessage(connfd, "END");
        if(res) mysql_free_result(res);
        return;
    }

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        snprintf(message, sizeof(message), "%s. %s", row[0], row[1]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

void handleShowCinemaByFilm(MYSQL *conn, int connfd) {
    char film_id[32] = {0};
    char query[512];
    char message[1024];

    char *tmp = strtok(NULL, "\r\n");
    if (!tmp) return;
    strcpy(film_id, tmp);

    sprintf(query,
        "SELECT DISTINCT c.id, c.name "
        "FROM cinemas c "
        "JOIN rooms r ON r.cinema_id = c.id "
        "JOIN showtimes st ON st.room_id = r.id "
        "WHERE st.film_id = %s "
        "ORDER BY c.id",
        film_id
    );

    if (mysql_query(conn, query) != 0) {
        sendMessage(connfd, "Cant load the list of cinemas!");
        sendMessage(connfd, "END");
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        sendMessage(connfd, "No cinemas available!");
        sendMessage(connfd, "END");
        if(res) mysql_free_result(res);
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        snprintf(message, sizeof(message), "%s. %s", row[0], row[1]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

void handleShowTimeByFilmCinema(MYSQL *conn, int connfd) {
    char film_id[32] = {0};
    char cinema_id[32] = {0};
    char query[512];
    char message[1024];

    char *tmp1 = strtok(NULL, "\r\n");
    char *tmp2 = strtok(NULL, "\r\n");
    if (!tmp1 || !tmp2) return;

    strcpy(film_id, tmp1);
    strcpy(cinema_id, tmp2);

    sprintf(query,
        "SELECT st.id, st.start_time, st.end_time "
        "FROM showtimes st "
        "JOIN rooms r ON st.room_id = r.id "
        "WHERE st.film_id = %s AND r.cinema_id = %s "
        "ORDER BY st.start_time",
        film_id, cinema_id
    );

    if (mysql_query(conn, query) != 0) {
        sendMessage(connfd, "Cant load the list of showtimes!");
        sendMessage(connfd, "END");
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        sendMessage(connfd, "No showtimes available!");
        sendMessage(connfd, "END");
        if(res) mysql_free_result(res);
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        snprintf(message, sizeof(message), "%s. %s - %s",
                 row[0], row[1], row[2]);
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
    mysql_free_result(res);
}

/* 
    Only get seats in the room of that showtime
 */
void handleShowSeat(MYSQL *conn, int connfd) {
    char showtime_id[255] = {0};
    char query[512];
    char message[2048];

    char *tmp = strtok(NULL, "\r\n");
    if(tmp) strcpy(showtime_id, tmp);


    sprintf(query,
        "SELECT s.id, s.seat_number, s.row_name "
        "FROM seats s "
        "WHERE s.room_id = ("
        "   SELECT room_id FROM showtimes WHERE id = %s"
        ") "
        "AND s.id NOT IN ("
        "   SELECT seat_id FROM tickets WHERE showtime_id = %s"
        ") "
        "ORDER BY s.row_name, s.seat_number", 
        showtime_id, showtime_id);

    if(mysql_query(conn, query) != 0){
        sendMessage(connfd, "Cant load the list of seats!");
        sendMessage(connfd, "END");
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendMessage(connfd, "No seats available!");
        sendMessage(connfd, "END");
        if(res) mysql_free_result(res);
        return;
    }

    strcpy(message, "=== Available Seats ===\n");
    MYSQL_ROW row;
    int count = 0;
    while((row = mysql_fetch_row(res))){
        char line[128];
        snprintf(line, sizeof(line), "SEAT|%s|%s%s", row[0], row[1], row[2]);
        sendMessage(connfd, line);
    }

    // sendMessage(connfd, message);
    sendMessage(connfd, "END");
    mysql_free_result(res);
}

void handleBookTicket(
    MYSQL *conn,
    int connfd,
    char *username,
    char *showtime_id,
    char *seat_id
) {
    int user_id = getUserIdByUsername(conn, username);
    char query[512];

    mysql_query(conn, "START TRANSACTION");

    sprintf(query,
        "INSERT INTO tickets(user_id, showtime_id, seat_id) "
        "VALUES(%d, %s, %s)",
        user_id, showtime_id, seat_id
    );

    if (mysql_query(conn, query) == 0) {
        mysql_query(conn, "COMMIT");
        sendResult(connfd, BOOK_TICKET_SUCCESS);
    } else {
        mysql_query(conn, "ROLLBACK");
        sendResult(connfd, BOOK_TICKET_FAIL);
    }
}

/*----------END BOOK TICKET--------*/