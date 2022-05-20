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

#include <timer/timer.h>
#include <unistd.h>

static void sig_handler(int sig) {
  saws_log("Received signal %d: %s", sig, strsignal(sig));
  stop_server(0);
}

static void welcome() {
  printf("SAWS - Simple Aircraft War Server (ver. 0.1)\n");
  printf("(C) Hans WAN. Licensed under MIT license.\n");
  printf("A part of coursework of OOP lecture, HIT Shenzhen.\n");
}

void print(int session_id, int cnt) {
  saws_log("%d, %d", session_id, cnt);
}

static void bind_signal() {
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

