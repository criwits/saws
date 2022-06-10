/**
 * @file main.c
 * @brief 程序入口
 * @author hans
 * @version 0.1.0
 */
#include <common.h>
#include <signal.h>

#include <engine/power.h>
#include "engine/lws.h"
#include <unistd.h>

static void sig_handler(int sig) {
  saws_log("Received signal %d: %s", sig, strsignal(sig));
  stop_server(0);
}

static inline void welcome() {
  printf("SAWS - Simple Aircraft War Server (ver. 0.1)\n");
  printf("(C) Hans WAN. Licensed under MIT license.\n");
  printf("A part of coursework of OOP lecture, HIT Shenzhen.\n");
  printf("\n[AD] " ANSI_BG_BLUE ANSI_FG_WHITE "来哈工大深圳" ANSI_NONE " " ANSI_BG_RED ANSI_FG_WHITE "体验非凡人生" ANSI_NONE "\n");
}

static inline void bind_signal() {
  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);
}

/**
 * 主函数
 * @param argc 参数个数
 * @param argv 参数列表
 * @return
 */
int main(int argc, const char *argv[]) {
  welcome();
  bind_signal();
  init_server(argc, argv);
  lws_loop();
  return 0;
}

