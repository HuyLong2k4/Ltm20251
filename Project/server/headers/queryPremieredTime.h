#ifndef QUERYPREMIERTIME_H
#define QUERYPREMIERTIME_H

#include <mysql/mysql.h>
#include "../../lib/premieredTime/functionPremieredTime.h"

void selectPremieredTime(MYSQL *connection, nodePremieredTime* h, premieredTime x);

#endif
