#ifndef QUERYCINEMA_H
#define QUERYCINEMA_H

#include <mysql/mysql.h>
#include "../../lib/cinemas/functionCinema.h"

void selectCinema(MYSQL *connection, nodeCinema* h, cinema x);

#endif