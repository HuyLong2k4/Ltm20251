#ifndef QUERYCATEGORY_H
#define QUERYCATEGORY_H

#include <mysql/mysql.h>
#include <string.h>
#include "../../lib/categories/functionCategory.h"

void selectCategory(MYSQL *connection, nodeCategory* h, category x);

#endif