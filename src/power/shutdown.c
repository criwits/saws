/**
 * @file shutdown.c
 * @brief 用来停止服务器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <database/mysql.h>
#include "engine/lws.h"

void stop_server(int status) {
  db_close();
  lws_stop();

  saws_log("Bye");
  exit(status);
}