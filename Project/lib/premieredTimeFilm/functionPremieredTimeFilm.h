#ifndef FUNCPREMIEREDTIMEFILM_H
#define FUNCPREMIEREDTIMEFILM_H

#include "premieredTimeFilm.h"

int checkEmptyListPremieredTimeFilm(nodePremieredTimeFilm list);

void addNewNodePremieredTimeFilm(nodePremieredTimeFilm* head, premieredTimeFilm x);

void addNodePremieredTimeFilm(nodePremieredTimeFilm* head, premieredTimeFilm x);

int *searchPremieredTimeFilm(nodePremieredTimeFilm head, unsigned long cinema_id_search);
int searchPremieredTimeFilmToPost(nodePremieredTimeFilm head, unsigned long film_id_search, unsigned long cinema_id_search, unsigned long premiered_time_id_search, char *date);

#endif