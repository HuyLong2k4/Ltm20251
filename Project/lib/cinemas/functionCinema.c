#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "cinema.h"

int checkEmptyListCinema(nodeCinema list){
	return list == NULL;
}

void addNewNodeCinema(nodeCinema* head, cinema x){
    struct NodeCinema* p = (struct NodeCinema*)malloc(sizeof(struct NodeCinema));
    p->data = x;
    p->next = NULL;
    if(checkEmptyListCinema(*head)){
        *head = p;
    }
}

void addNodeCinema(nodeCinema* head, cinema x){
    if(checkEmptyListCinema(*head)){
        addNewNodeCinema(head, x);
    }else{
        struct NodeCinema* a = *head;
        while(a->next != NULL){
            a = a->next;
        }
        struct NodeCinema* p = (struct NodeCinema*)malloc(sizeof(struct NodeCinema));
        p->data = x;
        p->next = NULL;
        a->next = p;
    }
}

int searchCimema(nodeCinema head, char name[])
{
    if (checkEmptyListCinema(head))
    {
        return 0;
    }
    else
    {
        struct NodeCinema *p = head;
        while (p != NULL)
        {
            if (strcmp(name, p->data.name) == 0)
            {
                return p->data.id;
            }
            p = p->next;
        }
    }
    return -1;
}

char *displayCinema(nodeCinema head)
{
    if (checkEmptyListCinema(head))
    {
        printf("Empty list\n");
        return "Empty list\n";
    }
    else
    {
        struct NodeCinema *p = head;
        int bufferSize = 2048;
        char *message = (char *)malloc(bufferSize * sizeof(char));

        strcpy(message, "");

        while (p != NULL)
        {
            char temp[512];
            sprintf(temp, "%ld %s\n", p->data.id, p->data.name);
            strcat(message, temp);

            p = p->next;
        }

        return message;
    }
}
