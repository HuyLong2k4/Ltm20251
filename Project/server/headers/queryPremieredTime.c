#include <mysql/mysql.h>
#include <string.h>
#include "../../lib/premieredTime/functionPremieredTime.h"


void selectPremieredTime(MYSQL *connection, nodePremieredTime* h, premieredTime x){
    mysql_query(connection, "SELECT * FROM premiered_time");
    MYSQL_RES *result = mysql_store_result(connection);
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        x.id = strtoul(row[0], NULL, 10);
        strcpy(x.start_time, row[1]);
        strcpy(x.finish_time, row[2]);

        addNodePremieredTime(h, x);
    }

    mysql_free_result(result);
}
