#ifndef FUNCTIONFILM_H
#define FUNCTIONFILM_H

#include "film.h"

int checkEmptyListFilm(nodeFilm list);

void addNewNodeFilm(nodeFilm* head, film x);

void addNodeFilm(nodeFilm* head, film x);

int searchTitle(nodeFilm head, char title[], nodeFilm* addf);

char *searchFilmFollowCategoryID(nodeFilm head, int category_id);

char *searchFilmFollowID(nodeFilm head, int id);

char *displayFilm(nodeFilm head);

#endif
