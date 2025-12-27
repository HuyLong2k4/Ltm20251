#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/socket/socket.h"
#include "../../lib/messages/message.h"
#include "menu.h"

#define MAX_BUFFER 1024

void clearKeyboardBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {

    }
}

void handleLogout(int sockfd, char *message);
void handleRegister(int sockfd, char *message);
void handleChangePassword(int sockfd, char *username, char *message);


/*----------BOOK TICKET--------*/

void handleBookTicket(int sockfd);

/*----------END BOOK TICKET--------*/


/*----------FIND FILM--------*/

void handleSearchByTitle(int sockfd);

/*----------END FIND FILM--------*/


/*----------ADD FILM--------*/

void handleAddNewFilm(int sockfd);

/*----------END ADD FILM--------*/


/*----------BROWSE FILM--------*/

void handleBrowseFollowCategory(int sockfd);
void handleBrowseFollowCinema(int sockfd);
void handleBrowseFollowShowTime(int sockfd);
void handleBrowseFilm(int sockfd);

/*----------END BROWSE FILM--------*/





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
                handleBrowseFilm(sockfd);
                break;
            }
            case 2:{
                // char *title;
                handleSearchByTitle(sockfd);
                break;
            }
            case 3:{
                handleBookTicket(sockfd);
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

/*----------BOOK TICKET--------*/
void handleBookTicket(int sockfd) {
    int choose;
    do{
        bookTicket(); // Menu: 1.Book Ticket 2.Go Back
        printf("Choice: ");
        scanf("%d", &choose);
        clearKeyboardBuffer();
        
        switch(choose){
            case 1:{
                char film_id[255];
                char cinema_id[255];
                char showtime_id[255];
                char seat_id[255];
                char message[2048];

                // Bước 1: Hiển thị danh sách phim
                printf("\n=== SELECT FILM ===\n");
                memset(message, 0, sizeof(message));
                sprintf(message, "SHOW_FILMS\r\n");
                sendMessage(sockfd, message);
                
                while (1) {
                    recvMessage(sockfd, message);
                    if (strcmp(message, "END") == 0) break;
                    printf("%s\n", message);
                }
                
                printf("Enter film ID: ");
                fgets(film_id, sizeof(film_id), stdin);
                film_id[strcspn(film_id, "\n")] = 0;
                
                // Bước 2: Hiển thị rạp chiếu phim đó
                printf("\n=== SELECT CINEMA ===\n");
                memset(message, 0, sizeof(message));
                sprintf(message, "SHOW_CINEMA_BY_FILM\r\n%s\r\n", film_id);
                sendMessage(sockfd, message);
                
                while (1) {
                    recvMessage(sockfd, message);
                    if (strcmp(message, "END") == 0) break;
                    printf("%s\n", message);
                }
                
                printf("Enter cinema ID: ");
                fgets(cinema_id, sizeof(cinema_id), stdin);
                cinema_id[strcspn(cinema_id, "\n")] = 0;

                // Bước 3: Hiển thị suất chiếu
                printf("\n=== SELECT SHOWTIME ===\n");
                memset(message, 0, sizeof(message));
                sprintf(message, "SHOW_TIME_BY_FILM_CINEMA\r\n%s\r\n%s\r\n", 
                        film_id, cinema_id);
                sendMessage(sockfd, message);
                
                while (1) {
                    recvMessage(sockfd, message);
                    if (strcmp(message, "END") == 0) break;
                    printf("%s\n", message);
                }
                
                printf("Enter showtime ID: ");
                fgets(showtime_id, sizeof(showtime_id), stdin);
                showtime_id[strcspn(showtime_id, "\n")] = 0;

                // Bước 4: Hiển thị ghế trống
                printf("\n=== SELECT SEAT ===\n");
                memset(message, 0, sizeof(message));
                sprintf(message, "SHOW_SEATS\r\n%s\r\n", showtime_id);
                sendMessage(sockfd, message);
                
                int seat_available = 0;

                while (1) {
                    recvMessage(sockfd, message);
                    if (strcmp(message, "END") == 0) break;
                    printf("%s\n", message);
                    if (strncmp(message, "SEAT|", 5) == 0) {
                        seat_available = 1;
                        char *seat_id = strtok(message + 5, "|");
                        char *seat_name = strtok(NULL, "|");
                        if (seat_id && seat_name) {
                            printf("ID: %-4s  Seat: %s\n", seat_id, seat_name);
                        }
                    }
                    else if (strncmp(message, "INFO|", 5) == 0) {
                        printf("%s\n", message + 5);
                    }
                }
                if (!seat_available) {
                    break;
                }
                printf("Enter seat ID: ");
                fgets(seat_id, sizeof(seat_id), stdin);
                seat_id[strcspn(seat_id, "\n")] = 0;

                // Bước 5: Đặt vé
                memset(message, 0, sizeof(message));
                sprintf(message, "BOOK_TICKET\r\n%s\r\n%s\r\n", 
                        showtime_id, seat_id);
                sendMessage(sockfd, message);

                int result = recvResult(sockfd);
                if (result == BOOK_TICKET_SUCCESS) {
                    printf("\nBOOK TICKET SUCCESS!\n");
                }
                else if (result == BOOK_TICKET_FAIL) {
                    printf("\nBOOK TICKET FAILED! (Seat already booked or system error)\n");
                }
                break;
            }
            case 2:{
                choose = 0;
                break;
            }
            default:
                printf("Invalid choice!\n");
        }
    } while(choose != 0);
}

/*----------END BOOK TICKET--------*/


/*----------FIND FILM--------*/
void handleSearchByTitle(int sockfd){
    char title[255] = {0};
    char message[1024] = {0};
    
    printf("\nEnter film title to search: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;
    
    sprintf(message, "SEARCH_FILM_BY_TITLE\r\n%s\r\n", title);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if(result == FIND_FILM_SUCCESS){
        printf("\n=== SEARCH RESULTS ===\n");
        while(1){
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if(strcmp(message, "END") == 0) break;
            printf("%s\n", message);
        }
    }else if(result == FIND_FILM_FAIL){
        printf("\nNo films found!\n");
    }
}
/*----------END FIND FILM--------*/


/*----------ADD FILM (ADMIN)--------*/
void handleAddNewFilm(int sockfd){
    char title[255];
    char category_id[255];
    char show_time[255];
    char description[2048];
    char message[4096];

    printf("\n=== ADD NEW FILM ===\n");
    
    // Show category list
    memset(message, 0, sizeof(message));
    sprintf(message, "SHOW_CATEGORIES\r\n");
    sendMessage(sockfd, message);
    
    // Read until "END"
    while(1){
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if(strcmp(message, "END") == 0) break;
        printf("%s\n", message);
    }
    
    printf("Enter film title: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;
    
    printf("Enter category ID: ");
    fgets(category_id, sizeof(category_id), stdin);
    category_id[strcspn(category_id, "\n")] = 0;
    
    printf("Enter duration (minutes): ");
    fgets(show_time, sizeof(show_time), stdin);
    show_time[strcspn(show_time, "\n")] = 0;
    
    printf("Enter description: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = 0;
    
    memset(message, 0, sizeof(message));
    sprintf(message, "ADD_FILM\r\n%s\r\n%s\r\n%s\r\n%s\r\n", 
            title, category_id, show_time, description);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if(result == ADD_FILM_SUCCESS){
        printf("\nADD FILM SUCCESS!\n");
    }else if(result == ADD_FILM_FAIL){
        printf("\nADD FILM FAILED!\n");
    }
}
/*----------END ADD FILM--------*/


/*----------BROWSE FILM--------*/

// Browse theo thể loại
void handleBrowseFollowCategory(int sockfd){
    char category_id[255];
    char message[2048];

    printf("\n=== BROWSE FILM BY CATEGORY ===\n");
    
    // Show category list
    memset(message, 0, sizeof(message));
    sprintf(message, "SHOW_CATEGORIES\r\n");
    sendMessage(sockfd, message);
    
    // Read until "END"
    while(1){
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if(strcmp(message, "END") == 0) break;
        printf("%s\n", message);
    }

    printf("Enter category ID: ");
    fgets(category_id, sizeof(category_id), stdin);
    category_id[strcspn(category_id, "\n")] = 0;

    memset(message, 0, sizeof(message));
    sprintf(message, "BROWSE_BY_CATEGORY\r\n%s\r\n", category_id);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if(result == BROWSE_CATEGORY_SUCCESS){
        printf("\n=== FILM LIST ===\n");
        while(1){
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if(strcmp(message, "END") == 0) break;
            printf("%s\n", message);
        }
    } else {
        printf("\nNo films found in this category!\n");
    }
}

// Browse theo rạp
void handleBrowseFollowCinema(int sockfd){
    char cinema_id[255];
    char message[2048];

    printf("\n=== BROWSE FILM BY CINEMA ===\n");
    
    memset(message, 0, sizeof(message));
    sprintf(message, "SHOW_CINEMAS\r\n");
    sendMessage(sockfd, message);
    
    // Read until "END"
    while(1){
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if(strcmp(message, "END") == 0) break;
        printf("%s\n", message);
    }

    printf("Enter cinema ID: ");
    fgets(cinema_id, sizeof(cinema_id), stdin);
    cinema_id[strcspn(cinema_id, "\n")] = 0;

    memset(message, 0, sizeof(message));
    sprintf(message, "BROWSE_BY_CINEMA\r\n%s\r\n", cinema_id);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if(result == BROWSE_THEATER_SUCCESS){
        printf("\n=== FILM LIST ===\n");
        while(1){
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if(strcmp(message, "END") == 0) break;
            printf("%s\n", message);
        }
    } else {
        printf("\nNo films found in this cinema!\n");
    }
}

// Browse theo giờ chiếu
void handleBrowseFollowShowTime(int sockfd){
    char time_slot[255];
    char message[2048];

    printf("\n=== SELECT FILM BY SHOWTIME ===\n");
    
    // Hiển thị danh sách giờ chiếu
    memset(message, 0, sizeof(message));
    sprintf(message, "SHOW_PREMIERED_TIME\r\n");
    sendMessage(sockfd, message);
    
    // Read until "END"
    while(1){
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if(strcmp(message, "END") == 0) break;
        printf("%s\n", message);
    }

    printf("Enter showtime (HH:MM, e.g., 09:00): ");
    fgets(time_slot, sizeof(time_slot), stdin);
    time_slot[strcspn(time_slot, "\n")] = 0;

    memset(message, 0, sizeof(message));
    sprintf(message, "BROWSE_BY_TIME\r\n%s\r\n", time_slot);
    sendMessage(sockfd, message);

    int result = recvResult(sockfd);
    if(result == BROWSE_TIME_SUCCESS){
        printf("\n=== FILM LIST ===\n");
        while(1){
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if(strcmp(message, "END") == 0) break;
            printf("%s\n", message);
        }
    } else {
        printf("\nNo films found in this time slot!\n");
    }
}

void handleBrowseFilm(int sockfd){
    int choice;
    do{
        browseFilm();
        printf("Choice: ");
        scanf("%d", &choice);
        clearKeyboardBuffer();
        
        switch(choice){
            case 1: handleBrowseFollowCategory(sockfd); break;
            case 2: handleBrowseFollowCinema(sockfd); break;
            case 3: choice = 0; break;
            default: printf("Invalid choice!\n");
        }
    } while(choice != 0);
}

/*----------END BROWSE FILM--------*/
