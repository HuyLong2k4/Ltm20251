#include <stdio.h>
#include <string.h>

char* getTypeMessage(char *message){
    return strtok(message, "\r\n");
}

void makeLoginMessage(char *username, char *password, char *message){
    strcpy(message, "LOGIN\r\n");
    strcat(message, username);
    strcat(message, "\r\n");
    strcat(message, password);
    strcat(message, "\r\n");
}

void getLoginMessage(char *username, char *password){
    char *tmp_username = strtok(NULL, "\r\n");
    char *tmp_password = strtok(NULL, "\r\n");
    if(tmp_username) strcpy(username, tmp_username);
    if(tmp_password) strcpy(password, tmp_password);
}

void makeRegisterMessage(char *name,char *username, char *password, char *message){
    strcpy(message, "REGISTER\r\n");
    strcat(message, name);
    strcat(message, "\r\n");
    strcat(message, username);
    strcat(message, "\r\n");
    strcat(message, password);
    strcat(message, "\r\n");
}

void getRegisterMessage(char *name, char *username, char *password){
    char *tmp_name = strtok(NULL, "\r\n");
    char *tmp_username = strtok(NULL, "\r\n");
    char *tmp_password = strtok(NULL, "\r\n");
    if(tmp_name) strcpy(name, tmp_name);
    if(tmp_username) strcpy(username, tmp_username);
    if(tmp_password) strcpy(password, tmp_password);
}

void makeChangePasswordMessage(char *username, char *oldPassword, char *newPassword, char *message) {
    strcpy(message, "CHANGE_PASSWORD\r\n");
    strcat(message, username);
    strcat(message, "\r\n");
    strcat(message, oldPassword);
    strcat(message, "\r\n");
    strcat(message, newPassword);
    strcat(message, "\r\n");
}

void getChangePasswordMessage(char *username, char *oldPassword, char *newPassword) {
    char *tmp_username = strtok(NULL, "\r\n");
    char *tmp_oldPassword = strtok(NULL, "\r\n");
    char *tmp_newPassword = strtok(NULL, "\r\n");
    if(tmp_username) strcpy(username, tmp_username);
    if(tmp_oldPassword) strcpy(oldPassword, tmp_oldPassword);
    if(tmp_newPassword) strcpy(newPassword, tmp_newPassword);
}

void makeLogoutMessage(char *message){
    strcpy(message, "LOGOUT\r\n");
}

void getLogoutMessage(char *username) {
    char *tmp_username = strtok(NULL, "\r\n");
    if(tmp_username) strcpy(username, tmp_username);
}

/*----------ADD FILM--------*/
void makeAddNewFilmMessage(
    char *title,
    char *category_id,
    char *show_time,
    char *description,
    char *message
){
    strcpy(message, "ADD_FILM\r\n");
    strcat(message, title);
    strcat(message, "\r\n");
    strcat(message, category_id);
    strcat(message, "\r\n");
    strcat(message, show_time);
    strcat(message, "\r\n");
    strcat(message, description);
    strcat(message, "\r\n");
}
/*----------END ADD FILM--------*/

/*----------FIND FILM--------*/

void makeSearchFilmByTitleMessage(char *title, char *message){
    strcpy(message, "TITLE\r\n");
    strcat(message, title);
    strcat(message, "\r\n");
}
void getTitleFromMessage(char *title) {
    char *tmp_title = strtok(NULL, "\r\n");
    if(tmp_title) {
        strcpy(title, tmp_title);
    } else {
        title[0] = '\0';
    }
}
/*----------END FIND FILM--------*/

/*----------BROWSE FILM--------*/

void makeShowCategoryMessage(char *message) {
    strcpy(message, "SHOW_CATEGORY\r\n");
}

void makeShowCinemaMessage(char *message) {
    strcpy(message, "SHOW_CINEMA\r\n");
}

void makeShowPremieredTimeMessage(char *message) {
    strcpy(message, "SHOW_PREMIERED_TIME\r\n");
}

void makeBrowseFollowCategoryMessage(char *category_id, char *message){
    strcpy(message, "BROWSE_CATEGORY\r\n");
    strcat(message, category_id);
    strcat(message, "\r\n");
}

void makeBrowseFollowCinemaMessage(char *cinema_id, char *message){
    strcpy(message, "BROWSE_CINEMA\r\n");
    strcat(message, cinema_id);
    strcat(message, "\r\n");
}

void makeBrowseFollowPremieredTimeMessage(char *premiered_time_id, char *message){
    strcpy(message, "BROWSE_PREMIERED_TIME\r\n");
    strcat(message, premiered_time_id);
    strcat(message, "\r\n");
}

void getCategoryIDFromMessage(char *message, char *category_id) {
    char *line;
    line = strtok(message, "\r\n"); // tách type, BROWSE_CATEGORY
    line = strtok(NULL, "\r\n");    // tách ID
    if(line) strncpy(category_id, line, 255);
    else category_id[0] = '\0'; // phòng trường hợp null
}

void getCinemaIDFromMessage(char *message, char *cinema_id) {
    char *line;
    line = strtok(message, "\r\n"); // tách type, BROWSE_CINEMA
    line = strtok(NULL, "\r\n");    // tách ID
    if(line) strncpy(cinema_id, line, 255);
    else cinema_id[0] = '\0';
}

void getPremieredTimeIDFromMessage(char *message, char *premiered_time_id) {
    char *line;
    line = strtok(message, "\r\n"); // tách type, BROWSE_PREMIERED_TIME
    line = strtok(NULL, "\r\n");    // tách ID
    if(line) strncpy(premiered_time_id, line, 255);
    else premiered_time_id[0] = '\0';
}


/*----------END BROWSE FILM--------*/

/*----------BOOK TICKET--------*/
void makeShowFilmMessage(char *message) {
    // Client yêu cầu danh sách phim
    strcpy(message, "SHOW_FILM\r\n");
}
void makeShowCinemaByFilmMessage(char *film_id, char *message) {
    // Client yêu cầu danh sách rạp theo film đã chọn
    strcpy(message, "SHOW_CINEMA\r\n");
    strcat(message, film_id);
    strcat(message, "\r\n");
}
void makeShowTimeByFilmCinemaMessage(char *film_id, char *cinema_id, char *message) {
    // Client yêu cầu suất chiếu theo film + cinema
    
    strcpy(message, "SHOW_SHOWTIME\r\n");
    strcat(message, film_id);
    strcat(message, "\r\n");
    strcat(message, cinema_id);
    strcat(message, "\r\n");
}
void makeShowSeatMessage(char *showtime_id, char *message) {
    // Client yêu cầu danh sách ghế theo suất chiếu
    strcpy(message, "SHOW_SEAT\r\n");
    strcat(message, showtime_id);
    strcat(message, "\r\n");
}
void makeBookTicketMessage(char *showtime_id, char *seat_id, char *message) {
    // Client gửi yêu cầu đặt vé
    strcpy(message, "BOOK_TICKET\r\n");
    strcat(message, showtime_id);
    strcat(message, "\r\n");
    strcat(message, seat_id);
    strcat(message, "\r\n");
}

void getBookTicketMessage(char *showtime_id, char *seat_id) {
    char *tmp_showtime = strtok(NULL, "\r\n");
    char *tmp_seat = strtok(NULL, "\r\n");
    if (tmp_showtime) strcpy(showtime_id, tmp_showtime);
    if (tmp_seat) strcpy(seat_id, tmp_seat);
}



// void getBookTicketMessage(char *showtime_id, char *seat_id, char* message) {
//     char *saveptr;
//     strtok_r(message, "\r\n", &saveptr); // tách type, BOOK_TICKET

//     char *tmp_showtime = strtok_r(NULL, "\r\n", &saveptr);
//     char *tmp_seat = strtok_r(NULL, "\r\n", &saveptr);

//     if (tmp_showtime) strcpy(showtime_id, tmp_showtime);
//     if (tmp_seat) strcpy(seat_id, tmp_seat);
// }
/*----------END BOOK TICKET--------*/

// Manager control

void makeAnnounceFilmMessage(char *film_id, char *cinema_id, char *premiered_time_id, char *date, char *message) {
    strcpy(message, "POST_FILM\r\n");
    strcat(message, film_id);
    strcat(message, "\r\n");
    strcat(message, cinema_id);
    strcat(message, "\r\n");
    strcat(message, premiered_time_id);
    strcat(message, "\r\n");
    strcat(message, date);
    strcat(message, "\r\n");
} 

void getAnnounceFilmMessage(char *film_id, char *cinema_id, char *premiered_time_id, char *date) {
    char *tmp_film_id = strtok(NULL, "\r\n");
    char *tmp_cinema_id = strtok(NULL, "\r\n");
    char *tmp_premiered_time_id = strtok(NULL, "\r\n");
    char *tmp_date = strtok(NULL, "\r\n");

    if(tmp_film_id) strcpy(film_id, tmp_film_id);
    if(tmp_cinema_id) strcpy(cinema_id, tmp_cinema_id);
    if(tmp_premiered_time_id) strcpy(premiered_time_id, tmp_premiered_time_id);
    if(tmp_date) strcpy(date, tmp_date);
}
