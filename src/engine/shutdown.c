/**
 * @file shutdown.c
 * @brief 用来停止服务器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <database/mysql.h>

void stop_server() {
  db_close();
  saws_log("Bye");
  exit(0);
}