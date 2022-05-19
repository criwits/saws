/**
 * @file mysql.c
 * @brief MySQL 数据库连接器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <mariadb/mysql.h>
#include <engine/server.h>

static MYSQL *conn;

void db_init(const char *database, const char *username, const char *password) {
  saws_log("MySQL / MariaDB connector version: %s", mysql_get_client_info());

  if ((conn = mysql_init(NULL)) == NULL) {
    saws_err("MySQL / MariaDB connector failed to initialise");
    stop_server(-1);
  }

  if ((conn = mysql_real_connect(conn, "localhost", username, password, database, 0, NULL, 0)) == NULL) {
    saws_err("Cannot connect to database `%s`: %s", database, mysql_error(conn));
    stop_server(-1);
  }

  saws_log("Successfully connected to database `%s`", database);
}

void db_close() {
  mysql_close(conn);
  saws_log("Closed database connection");
}

MYSQL_RES *db_query(const char *query) {
  if(mysql_query(conn, query)) {
    saws_warn("Error(s) occurred when executing SQL query: %s", query);
    return NULL;
  }
  return mysql_store_result(conn);
}