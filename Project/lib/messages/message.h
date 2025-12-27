#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

char* getTypeMessage(char *message);

void makeLoginMessage(char *username, char *password, char *message);

void getLoginMessage(char *username, char *password);

void makeRegisterMessage(char *name,char *username, char *password, char *message);

void getRegisterMessage(char *name, char *username, char *password);

void makeChangePasswordMessage(char *username, char *oldPassword, char *newPassword, char *message);

void getChangePasswordMessage(char *username, char *oldPassword, char *newPassword);

void makeLogoutMessage(char *message);

void getLogoutMessage(char *username);

/*----------ADD FILM--------*/
void makeAddNewFilmMessage(
    char *title,
    char *category_id,
    char *show_time,
    char *description,
    char *message
);
/*----------END ADD FILM--------*/

/*----------FIND FILM--------*/
void makeSearchFilmByTitleMessage(char *title, char *message);
void getTitleFromMessage(char *title);
/*----------END FIND FILM--------*/

/*----------BROWSE FILM--------*/

void makeShowCategoryMessage(char *message);
void makeShowCinemaMessage(char *message);
void makeShowPremieredTimeMessage(char *message);

void makeBrowseFollowCategoryMessage(char *category_id, char *message);
void makeBrowseFollowCinemaMessage(char *cinema_id, char *message);
void makeBrowseFollowPremieredTimeMessage(char *premiered_time_id, char *message);

void getCategoryIDFromMessage(char *message, char *category_id);
void getCinemaIDFromMessage(char *message, char *cinema_id);
void getPremieredTimeIDFromMessage(char *message, char *premiered_time_id);
/*----------END BROWSE FILM--------*/

/*----------BOOK TICKET--------*/
void makeShowFilmMessage(char *message);

void makeShowCinemaByFilmMessage(char *film_id, char *message);

void makeShowTimeByFilmCinemaMessage(char *film_id, char *cinema_id, char *message);

void makeShowSeatMessage(char *showtime_id, char *message);

void makeBookTicketMessage(char *showtime_id, char *seat_id, char *message);

void getBookTicketMessage(char *showtime_id, char *seat_id);
/*----------END BOOK TICKET--------*/

void makeAnnounceFilmMessage(char *film_id, char *cinema_id, char *premiered_time_id, char *date, char *message);

void getAnnounceFilmMessage(char *film_id, char *cinema_id, char *premiered_time_id, char *date);

#ifdef __cplusplus
}
#endif

#endif