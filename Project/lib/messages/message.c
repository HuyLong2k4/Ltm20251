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

void makeRegisterMessage(char *name,char *username, char *password, char *message){
    strcpy(message, "REGISTER\r\n");
    strcat(message, name);
    strcat(message, "\r\n");
    strcat(message, username);
    strcat(message, "\r\n");
    strcat(message, password);
    strcat(message, "\r\n");
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


void makeLogoutMessage(char *message){
    strcpy(message, "LOGOUT\r\n");
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

