#ifndef SAWS_MYSQL_H
#define SAWS_MYSQL_H

#include <mariadb/mysql.h>

void db_init(const char *database, const char *username, const char *password);
void db_close();
MYSQL_RES *db_query(const char *query);

#endif //SAWS_MYSQL_H
