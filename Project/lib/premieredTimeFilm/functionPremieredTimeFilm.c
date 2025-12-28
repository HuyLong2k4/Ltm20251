#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "premieredTimeFilm.h"

int checkEmptyListPremieredTimeFilm(nodePremieredTimeFilm list){
	return list == NULL;
}

void addNewNodePremieredTimeFilm(nodePremieredTimeFilm* head, premieredTimeFilm x){
    struct NodePremieredTimeFilm* p = (struct NodePremieredTimeFilm*)malloc(sizeof(struct NodePremieredTimeFilm));
    p->data = x;
    p->next = NULL;
    if(checkEmptyListPremieredTimeFilm(*head)){
        *head = p;
    }
}

void addNodePremieredTimeFilm(nodePremieredTimeFilm* head, premieredTimeFilm x){
    if(checkEmptyListPremieredTimeFilm(*head)){
        addNewNodePremieredTimeFilm(head, x);
    }else{
        struct NodePremieredTimeFilm* a = *head;
        while(a->next != NULL){
            a = a->next;
        }
        struct NodePremieredTimeFilm* p = (struct NodePremieredTimeFilm*)malloc(sizeof(struct NodePremieredTimeFilm));
        p->data = x;
        p->next = NULL;
        a->next = p;
    }
}

// Tìm tất cả các film_id đang chiếu tại một rạp cụ thể (theo cinema_id)
int *searchPremieredTimeFilm(nodePremieredTimeFilm head, unsigned long cinema_id_search)
{
    int* arr = (int*)malloc(sizeof(int));
    int i = 0;

    if (checkEmptyListPremieredTimeFilm(head))
    {
        return 0;
    }
    struct NodePremieredTimeFilm *a = head;
    while (a != NULL)
    {
        if (a->data.cinema_id == cinema_id_search)
        {
            arr[i] = a->data.film_id;
            i++;
        }
        a = a->next;
    }
    return arr;
}

//Tìm kiếm lịch chiếu cụ thể theo film_id, cinema_id, premiered_time_id và ngày chiếu
int searchPremieredTimeFilmToPost(nodePremieredTimeFilm head, unsigned long film_id_search, unsigned long cinema_id_search, unsigned long premiered_time_id_search, char *date)
{
    int i = 0;

    if (checkEmptyListPremieredTimeFilm(head))
    {
        return 0;
    }
    struct NodePremieredTimeFilm *a = head;
    while (a != NULL)
    {
        if (a->data.film_id == film_id_search && a->data.cinema_id == cinema_id_search && a->data.premiered_time_id == premiered_time_id_search && strcmp(a->data.date, date) == 0)
        {
            i++;
        }
        a = a->next;
    }
    return i;
}

// Hiển thị toàn bộ danh sách lịch chiếu
char *displayPremieredTimeFilm(nodePremieredTimeFilm head) {
    if (checkEmptyListPremieredTimeFilm(head)) {
        printf("Empty list\n");
        return "Empty list\n";
    } else {
        struct NodePremieredTimeFilm *p = head;
        int bufferSize = 2048;
        char *message = (char *)malloc(bufferSize * sizeof(char));

        strcpy(message, "STT\tFilm ID\tPremiered Time ID\tCinema ID\n");

        while (p != NULL) {
            char temp[4096];
            sprintf(temp, "%ld\t%ld\t%ld\t%ld\n", p->data.id, p->data.film_id, p->data.premiered_time_id, p->data.cinema_id);
            strcat(message, temp);

            p = p->next;
        }

        return message;
    }
}
