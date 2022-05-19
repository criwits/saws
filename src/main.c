/**
 * @file main.c
 * @brief 程序入口
 * @author hans
 * @version 0.1.0
 */
#include <common.h>
#include <signal.h>

#include <engine/server.h>
#include <network/lws.h>

void sig_handler(int sig) {
  saws_log("Received signal %d: %s", sig, strsignal(sig));
  stop_server(0);
}

/**
 * 主函数
 * 传参只为配置文件，即：
 *   saws -c config.json
 * @param argc 参数个数
 * @param argv 参数列表
 * @return
 */
int main(int argc, const char *argv[]) {
  printf("SAWS - Simple Aircraft War Server (ver. 0.1)\n");
  printf("(C) Hans WAN. Licensed under MIT license.\n");

  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);

  init_server(argc, argv);

  lws_loop();

  return 0;
}


