#ifndef SAWS_API_H
#define SAWS_API_H

#include <mariadb/mysql.h>

int query_user(const char *username, const char *password);
void insert_ranking(int uid, int score, int enroll_date);
MYSQL_RES *get_ranking_json();

#endif //SAWS_API_H
