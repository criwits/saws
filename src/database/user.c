/**
 * @file user.c
 * @brief 用户相关 DAO
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <database/mysql.h>

#define MAX_QUERY_PATTERN_LENGTH 256
#define USER_QUERY_PATTERN "SELECT * FROM users WHERE username=\'%s\' AND password=\'%s\'"

/**
 * 验证用户名和密码
 * @param username
 * @param password
 * @return UID (-1 = Invalid)
 */
int query_user(const char *username, const char *password) {
  char buf[MAX_QUERY_PATTERN_LENGTH];
  sprintf(buf, USER_QUERY_PATTERN, username, password);
  MYSQL_RES *res = db_query(buf);

  if (res == NULL || (int) mysql_num_rows(res) == 0) {
    mysql_free_result(res);
    return -1;
  }

  if ((int) mysql_num_rows(res) > 1) {
    saws_warn("More than one user found with given user name and password");
    saws_warn("Please check database structure manually");
  }
  mysql_free_result(res);
  return atoi(mysql_fetch_row(res)[0]);
}