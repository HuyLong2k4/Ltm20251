#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include "../../lib/premieredTimeFilm/functionPremieredTimeFilm.h"
void selectPremieredTimeFilm(MYSQL *connection, nodePremieredTimeFilm* h, premieredTimeFilm x){
    mysql_query(connection, "SELECT * FROM premiered_time_film");
    MYSQL_RES *result = mysql_store_result(connection);
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        x.id = strtoul(row[0], NULL, 10);
        x.film_id = strtoul(row[1], NULL, 10);
        x.premiered_time_id = strtoul(row[2], NULL, 10);
        x.cinema_id = strtoul(row[3], NULL, 10);

         if (row[4] != NULL) {
            strncpy(x.date, row[4], sizeof(x.date) - 1);
            x.date[sizeof(x.date) - 1] = '\0';
        } else {
            x.date[0] = '\0';
        }

        addNodePremieredTimeFilm(h, x);
    }

    mysql_free_result(result);
}

void addPremieredTimeFilm(MYSQL *connection, premieredTimeFilm x){
    char query[4096];
    snprintf(query, sizeof(query), "INSERT INTO premiered_time_film(film_id, premiered_time_id, cinema_id, date) VALUES('%ld', %ld, %ld, '%s')", x.film_id, x.premiered_time_id, x.cinema_id, x.date);
    mysql_query(connection, query);
}
