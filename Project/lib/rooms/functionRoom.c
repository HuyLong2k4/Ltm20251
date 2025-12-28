#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "room.h"

int checkEmptyListRoom(nodeRoom list){
	return list == NULL;
}

void addNewNodeRoom(nodeRoom* head, room x){
    struct NodeRoom* p = (struct NodeRoom*)malloc(sizeof(struct NodeRoom));
    p->data = x;
    p->next = NULL;
    if(checkEmptyListRoom(*head)){
        *head = p;
    }
}

void addNodeRoom(nodeRoom* head, room x){
    if(checkEmptyListRoom(*head)){
        addNewNodeRoom(head, x);
    }else{
        struct NodeRoom* a = *head;
        while(a->next != NULL){
            a = a->next;
        }
        struct NodeRoom* p = (struct NodeRoom*)malloc(sizeof(struct NodeRoom));
        p->data = x;
        p->next = NULL;
        a->next = p;
    }
}
