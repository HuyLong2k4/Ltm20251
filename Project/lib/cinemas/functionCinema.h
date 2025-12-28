#ifndef FUNCTIONCINEMA_H
#define FUNCTIONCINEMA_H

#include "cinema.h"

int checkEmptyListCinema(nodeCinema list);

void addNewNodeCinema(nodeCinema* head, cinema x);

void addNodeCinema(nodeCinema* head, cinema x);

int searchCimema(nodeCinema head, char name[]);

char *displayCinema(nodeCinema head);

#endif