#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "queryUser.h"
#include "function.h"


#define LOGIN_SUCCESS_USER 1010
#define LOGIN_SUCCESS_MANAGER 1011
#define LOGIN_SUCCESS_ADMIN 1012
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
#define BOOK_TICKET_SUCCESS 1200
#define BOOK_TICKET_FAIL 2200
#define CHANGE_PASSWORD_SUCCESS 1110
#define CHANGE_PASSWORD_FAIL 2110
#define SHOW_ALL_USERS_SUCCESS 1300
#define SHOW_ALL_USERS_FAIL 2300
#define DELETE_USER_SUCCESS 1301
#define DELETE_USER_FAIL 2301
#define UPDATE_USER_ROLE_SUCCESS 1302
#define UPDATE_USER_ROLE_FAIL 2302

// void handleRequest( MYSQL *conn, char *type, int connfd, char *username, char *password, listLoginedAccount *arr, node *h);
void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password);
void handleLogout( int connfd, listLoginedAccount *arr, char *username);
void handleRegister( MYSQL *conn, int connfd, node *h);
void handleChangePassword( int connfd, MYSQL *conn, node *h);

void handleShowRoomsByCinema(MYSQL *conn, int connfd, char *cinema_id);
void handleShowShowtimesByRoom(MYSQL *conn, int connfd, char *room_id);
void handleAddShowTime(MYSQL *conn, int connfd, char *film_id, char *cinema_id, char *room_id, char *start_datetime);

void handleViewTickets(MYSQL *conn, int connfd, char *username);
void handleViewTicketDetail(MYSQL *conn, int connfd, char *ticket_id);


void handleLogin(int connfd, listLoginedAccount *arr, node *h, char *username, char *password){
    int check = checkLogin(*h, &username, password, arr);
    printf("Login check result: %d\n", check);

    if (check == -1) {
        // Login failed - wrong username/password
        sendResult(connfd, LOGIN_FAIL);
    }
    else if (check == 0) {
        // Admin role (role_id = 0)
        addToListLoginedAccount(arr, &username);
        sendResult(connfd, LOGIN_SUCCESS_ADMIN);
    }
    else if (check == 1) {
        // Manager role (role_id = 1)
        addToListLoginedAccount(arr, &username);
        sendResult(connfd, LOGIN_SUCCESS_MANAGER);
    }
    else if (check == 2) {
        // User role (role_id = 2)
        addToListLoginedAccount(arr, &username);
        sendResult(connfd, LOGIN_SUCCESS_USER);
    }
    else if (check == 3) {
        // Already logged in
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
    char query[1024];
    char message[2048];

    char *tmp = strtok(NULL, "\r\n");
    if(tmp) strcpy(showtime_id, tmp);


    sprintf(query,
        "SELECT s.id, s.row_name, s.seat_number "
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
    while((row = mysql_fetch_row(res))){
        char line[128];
        snprintf(line, sizeof(line), "SEAT|%s|%s-%s", row[0], row[1], row[2]);
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

void handleViewTickets(MYSQL *conn, int connfd, char *username) {
    char query[1024];
    char message[2048];

    sprintf(query, 
        "SELECT t.id, f.title, c.name, r.name, "
        "CONCAT(s.row_name, '-', s.seat_number) as seat, "
        "st.start_time, st.end_time, t.booked_at "
        "FROM tickets t "
        "JOIN users u ON t.user_id = u.id "
        "JOIN showtimes st ON t.showtime_id = st.id "
        "JOIN films f ON st.film_id = f.id "
        "JOIN rooms r ON st.room_id = r.id "
        "JOIN cinemas c ON r.cinema_id = c.id "
        "JOIN seats s ON t.seat_id = s.id "
        "WHERE u.username = '%s' "
        "ORDER BY st.start_time DESC", username);
    
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row;
        
        if (mysql_num_rows(result) == 0) {
            sprintf(message, "No tickets found.");
            sendMessage(connfd, message);
        } else {
            sprintf(message, "%-5s | %-20s | %-20s | %-10s | %-5s | %-20s | %-20s", 
                    "ID", "Film", "Cinema", "Room", "Seat", "Start Time", "Booked At");
            sendMessage(connfd, message);
            while ((row = mysql_fetch_row(result))) {
                sprintf(message, "%-5s | %-20s | %-20s | %-10s | %-5s | %-20s | %-20s", 
                        row[0], row[1], row[2], row[3], row[4], row[5], row[7]);
                sendMessage(connfd, message);
            }
            
            sprintf(message, "\nYou has total: %lu ticket(s)", mysql_num_rows(result));
            sendMessage(connfd, message);
        }
        mysql_free_result(result);
    } else {
        sprintf(message, "Failed to retrieve tickets.");
        sendMessage(connfd, message);
    }
    
    sendMessage(connfd, "END");
}
void handleViewTicketDetail(MYSQL *conn, int connfd, char *ticket_id) {

     char query[1024];
    char message[2048];
    
    sprintf(query,
            "SELECT u.name, u.username, f.title, c.name, c.address, "
            "r.name, r.total_seats, CONCAT(s.row_name, s.seat_number) as seat, "
            "st.start_time, st.end_time, t.booked_at, "
            "TIMESTAMPDIFF(HOUR, NOW(), st.start_time) as hours_until "
            "FROM tickets t "
            "JOIN users u ON t.user_id = u.id "
            "JOIN showtimes st ON t.showtime_id = st.id "
            "JOIN films f ON st.film_id = f.id "
            "JOIN rooms r ON st.room_id = r.id "
            "JOIN cinemas c ON r.cinema_id = c.id "
            "JOIN seats s ON t.seat_id = s.id "
            "WHERE t.id = %s",
            ticket_id);
    
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        
        if (mysql_num_rows(result) == 0) {
            sprintf(message, "Ticket not found!");
            sendMessage(connfd, message);
        } else {
            MYSQL_ROW row = mysql_fetch_row(result);
            
            sprintf(message, "\n=== TICKET DETAIL ===\n");
            sendMessage(connfd, message);
            
            sprintf(message, "Customer: %s (%s)", row[0], row[1]);
            sendMessage(connfd, message);
            
            sprintf(message, "\nFilm: %s", row[2]);
            sendMessage(connfd, message);
            
            sprintf(message, "\nCinema: %s | Address: %s", row[3], row[4]);
            sendMessage(connfd, message);
            
            sprintf(message, "\nRoom: %s | Capacity: %s seats |  Seat: %s", row[5], row[6], row[7]);
            sendMessage(connfd, message);
            
            sprintf(message, "\nShowtime: %s - %s", row[8], row[9]);
            sendMessage(connfd, message);
            
            sprintf(message, "\nBooked at: %s", row[10]);
            sendMessage(connfd, message);
        }
        mysql_free_result(result);
    }

    sendMessage(connfd, "END");
}



/*----------MANAGER--------*/

/*-----ADD FILM-----*/
void handleAddFilm(MYSQL *conn, int connfd, char *title, char *category_id, char *show_time) {
    char query[1024];
    char response[1024];

    sprintf(query, 
        "SELECT COUNT(*) FROM films WHERE LOWER(title) = LOWER('%s')", title);
    
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row && atoi(row[0]) > 0) {
            mysql_free_result(result);
            sprintf(response, "Film '%s' already exists.", title);
            sendMessage(connfd, response);
            return;
        }
        mysql_free_result(result);
    }
    // 2. Check category_id exists
    sprintf(query, 
        "SELECT COUNT(*) FROM categories WHERE id = %s", category_id);
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row && atoi(row[0]) == 0) {
            mysql_free_result(result);
            sprintf(response, "Category ID '%s' does not exist.", category_id);
            sendMessage(connfd, response);
            return;
        }
        mysql_free_result(result);
    }

    // 4. Insert new film
    sprintf(query, 
        "INSERT INTO films (title, category_id, show_time) VALUES ('%s', %s, '%s')", title, category_id, show_time);
    if (mysql_query(conn, query) == 0) {
        int film_id = mysql_insert_id(conn);
        sprintf(response, "Film '%s' added successfully with ID %d.", title, film_id);
        sendMessage(connfd, response);
        printf("[LOG] Added film: %s (ID: %d)\n", title, film_id);
    } else {
        sprintf(response, "Failed to add film '%s'", title);
        sendMessage(connfd, response);
        printf("[ERROR] Failed to add film: %s\n", title);
    }
}

/*-----END ADD FILM-----*/

/*-----ADD SHOW TIME-----*/

void handleShowRoomsByCinema(MYSQL *conn, int connfd, char *cinema_id) {
    char query[512];
    char message[1024];

    // Check if cinema exists
    sprintf(query, "SELECT COUNT(*) FROM cinemas WHERE id = %s", cinema_id);
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row && atoi(row[0]) == 0) {
            mysql_free_result(result);
            sprintf(message, "ERROR: Invalid cinema ID!");
            sendMessage(connfd, message);
            sendMessage(connfd, "END");
            return;
        }
        mysql_free_result(result);
    }

    // Show rooms in the cinema
    sprintf(query, 
        "SELECT r.id, r.name, r.total_seats "
        "FROM rooms r " 
        "WHERE r.cinema_id = %s AND r.status = 'ACTIVE' "
        "ORDER BY r.id",
        cinema_id);
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
    
        if (mysql_num_rows(result) == 0) {
            sprintf(message, "No rooms available in this cinema.");
            sendMessage(connfd, message);
        } else {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                sprintf(message, "ID: %s | %s | %s seats", 
                        row[0], row[1], row[2]);
                sendMessage(connfd, message);
            }
        }
        mysql_free_result(result);
    }
    else {
        sprintf(message, "Failed to retrieve rooms.");
        sendMessage(connfd, message);
    }

    sendMessage(connfd, "END");
}

void handleShowShowtimesByRoom(MYSQL *conn, int connfd, char *room_id) {
    char query[512];
    char message[1024];

    sprintf(query,
        "SELECT s.id, f.title, s.start_time, s.end-time, "
        "TIMESTAMPDIFF(MINUTE, s.start_time, s.end_time) AS duration "
        "FROM showtimes s "
        "JOIN films f ON s.film_id = f.id "
        "WHERE s.room_id = %s "
        "AND DATE(s.start_time) >= CURDATE() "
        "ORDER BY s.start_time", room_id);

    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);

        if (mysql_num_rows(result) == 0) {
            memset(message, 0, sizeof(message));
            sprintf(message, "No showtimes availale in this room.");
            sendMessage(connfd, message);
        }
        else {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                memset(message, 0, sizeof(message));
                sprintf(message, "ID: %s | Title: %s | %s - %s (%s minutes)", row[0], row[1], row[2], row[3], row[4]);
                sendMessage(connfd, message);
            }
        }
        mysql_free_result(result);
    }
    sendMessage(connfd, "END");
}
void handleAddShowTime(MYSQL *conn, int connfd, char *film_id, char *cinema_id, char *room_id, char *start_datetime) {
    char query[1024];
    char response[1024];
    char end_datetime[50];
    int duration;

    // 1. Check if film exists and get duration
    sprintf(query, "SELECT show_time FROM films WHERE id = %s", film_id);
    memset(response, 0, sizeof(response));
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (mysql_num_rows(result) == 0) {
            mysql_free_result(result);
            sprintf(response, "Invalid film ID");
            sendMessage(connfd, response);
            return;
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        duration = atoi(row[0]);
        mysql_free_result(result);
    }
    else {
        sprintf(response, "Database query error");
        sendMessage(connfd, response);
        return;
    }

    // 2. Check if cinema exist
    sprintf(query, "SELECT COUNT(*) FROM cinemas WHERE id = %s", cinema_id);
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row && atoi(row[0]) == 0) {
            mysql_free_result(result);
            memset(response, 0, sizeof(response));
            sprintf(response, "Invalid cinema ID");
            sendMessage(connfd, response);
            return;
        }
        mysql_free_result(result);
    }
    
    // 3. Check if room exits, activate anđ belongs to selected cinema
    sprintf(query, "SELECT cinema_id FROM rooms WHERE id = %s AND status = 'ACTIVE'", room_id);
    int room_cinema_id = 0;
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (mysql_num_rows(result) == 0) {
            mysql_free_result(result);
            memset(response, 0, sizeof(response));
            sprintf(response, "Invalid room ID or room is not active");
            sendMessage(connfd, response);
            return;
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        room_cinema_id = atoi(row[0]);
        mysql_free_result(result);

        if (room_cinema_id != atoi(cinema_id)) {
            memset(response, 0, sizeof(response));
            sprintf(response, "Room does not belong to the selected cinema");
            sendMessage(connfd, response);
            return;
        }
    }

    // 4. Calculate end_datetime
    sprintf(query, "SELECT DATE_ADD('%s', INTERVAL %d MINUTE)", start_datetime, duration);

    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        strcpy(end_datetime, row[0]);
        mysql_free_result(result);
    } 
    else {
        memset(response, 0, sizeof(response));
        sprintf(response, "Invalid start datetime format");
        sendMessage(connfd, response);
        return;
    }

    // 5. Check for scheduling conflicts (overlapping showtimes in the same room 15 minutes buffer)
    // Movie A
    // Movie B     
    // Movie A ends -- 15 minutes -- Movie C starts -- duration -- Movie C ends -- 15 minutes -- Movie B starts 
    sprintf(query, 
        "SELECT COUNT(*) FROM showtimes "
        "WHERE room_id = %s "
        "AND NOT (end_time <= DATE_SUB('%s', INTERVAL 15 MINUTE) OR start_time >= DATE_ADD('%s', INTERVAL 15 MINUTE))", 
        room_id, start_datetime, end_datetime);
    
    if (mysql_query(conn, query) == 0) {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);    
        if (row && atoi(row[0]) > 0) {
            mysql_free_result(result);
            memset(response, 0, sizeof(response));
            sprintf(response, "Has scheduling conflict in the selected room");
            sendMessage(connfd, response);
            return;
        }
        mysql_free_result(result);
    }

    // 6. insert new showtime
    sprintf(query, "INSERT INTO showtimes (film_id, room_id, start_time, end_time) VALUES (%s, %s, '%s', '%s')",
            film_id, room_id, start_datetime, end_datetime);
    if (mysql_query(conn, query) == 0) {
        int showtime_id = mysql_insert_id(conn);
        sprintf(query, 
            "SELECT c.name, r.name FROM cinemas c "
            "JOIN rooms r ON r.cinema_id = c.id "
            "WHERE c.id = %s AND r.id = %s",
            cinema_id, room_id);
        char cinema_name[255] = "empty";
        char room_name[255] = "empty";
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *result = mysql_store_result(conn);
            if (mysql_num_rows(result) > 0) {
                MYSQL_ROW row = mysql_fetch_row(result);
                strcpy(cinema_name, row[0]);
                strcpy(room_name, row[1]);
            }
            mysql_free_result(result);
        }
        sprintf(response, "Showtime added successfully!");
        sendMessage(connfd, response);
        printf("[LOG] Added showtime (ID: %d) for film %s at %s - %s - %s\n", 
               showtime_id, film_id, cinema_name, room_name, start_datetime);
    }
    else {
        sprintf(response, "Failed to add showtime!");
        sendMessage(connfd, response);
        printf("[ERROR] Failed to add showtime: %s\n", mysql_error(conn));
    }
}

/*-----END ADD SHOW TIME-----*/


/*----------END MANAGER--------*/

/*----------ADMIN MANAGEMENT--------*/

void handleShowAllUsers(MYSQL *conn, int connfd) {
    char query[512];
    char message[1024];
    
    sprintf(query, "SELECT id, name, username, role_id FROM users ORDER BY id");
    
    if(mysql_query(conn, query) != 0){
        printf("[DEBUG] Query failed: %s\n", mysql_error(conn));
        sendResult(connfd, SHOW_ALL_USERS_FAIL);
        sendMessage(connfd, "END");
        return;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        printf("[DEBUG] No results or empty result set\n");
        sendResult(connfd, SHOW_ALL_USERS_FAIL);
        sendMessage(connfd, "END");
        if(res) mysql_free_result(res);
        return;
    }
    
    int row_count = mysql_num_rows(res);
    printf("[DEBUG] Found %d users, sending result code %d\n", row_count, SHOW_ALL_USERS_SUCCESS);
    sendResult(connfd, SHOW_ALL_USERS_SUCCESS);
    
    MYSQL_ROW row;
    int i = 0;
    while((row = mysql_fetch_row(res))){
        const char* role_name;
        int role_id = atoi(row[3]);
        
        switch(role_id) {
            case 0: role_name = "Admin"; break;
            case 1: role_name = "Manager"; break;
            case 2: role_name = "User"; break;
            default: role_name = "Unknown";
        }
        
        snprintf(message, sizeof(message), "%s|%s|%s|%s", 
                 row[0], row[1], row[2], role_name);
        printf("[DEBUG] Sending user %d: %s\n", ++i, message);
        sendMessage(connfd, message);
    }
    
    printf("[DEBUG] Sending END marker\n");
    sendMessage(connfd, "END");
    mysql_free_result(res);
}

void handleDeleteUser(MYSQL *conn, int connfd) {
    char *user_id_str = strtok(NULL, "\r\n");
    printf("[DEBUG] Delete user - user_id_str: %s\n", user_id_str ? user_id_str : "NULL");
    
    if(!user_id_str) {
        printf("[DEBUG] Missing user_id parameter\n");
        sendResult(connfd, DELETE_USER_FAIL);
        return;
    }
    
    int user_id = atoi(user_id_str);
    char query[512];
    
    // Check if user exists
    sprintf(query, "SELECT id FROM users WHERE id = %d", user_id);
    if(mysql_query(conn, query) != 0){
        sendResult(connfd, DELETE_USER_FAIL);
        return;
    }
    
    MYSQL_RES *res = mysql_store_result(conn);
    if(!res || mysql_num_rows(res) == 0){
        sendResult(connfd, DELETE_USER_FAIL);
        if(res) mysql_free_result(res);
        return;
    }
    mysql_free_result(res);
    
    // Delete user
    sprintf(query, "DELETE FROM users WHERE id = %d", user_id);
    if(mysql_query(conn, query) == 0){
        sendResult(connfd, DELETE_USER_SUCCESS);
    } else {
        sendResult(connfd, DELETE_USER_FAIL);
    }
}

void handleChangeUserRole(MYSQL *conn, int connfd) {
    char *user_id_str = strtok(NULL, "\r\n");
    char *new_role_str = strtok(NULL, "\r\n");
    
    printf("[DEBUG] Change role - user_id_str: %s, new_role_str: %s\n", 
           user_id_str ? user_id_str : "NULL", 
           new_role_str ? new_role_str : "NULL");
    
    if(!user_id_str || !new_role_str) {
        printf("[DEBUG] Missing parameters\n");
        sendResult(connfd, UPDATE_USER_ROLE_FAIL);
        return;
    }
    
    int user_id = atoi(user_id_str);
    int new_role = atoi(new_role_str);
    
    printf("[DEBUG] Parsed - user_id: %d, new_role: %d\n", user_id, new_role);
    
    // Validate role
    if(new_role < 0 || new_role > 2) {
        printf("[DEBUG] Invalid role: %d\n", new_role);
        sendResult(connfd, UPDATE_USER_ROLE_FAIL);
        return;
    }
    
    char query[512];
    sprintf(query, "UPDATE users SET role_id = %d WHERE id = %d", new_role, user_id);
    
    printf("[DEBUG] Executing query: %s\n", query);
    if(mysql_query(conn, query) == 0 && mysql_affected_rows(conn) > 0){
        printf("[DEBUG] Update successful, affected rows: %llu\n", mysql_affected_rows(conn));
        sendResult(connfd, UPDATE_USER_ROLE_SUCCESS);
    } else {
        printf("[DEBUG] Update failed: %s\n", mysql_error(conn));
        sendResult(connfd, UPDATE_USER_ROLE_FAIL);
    }
}
