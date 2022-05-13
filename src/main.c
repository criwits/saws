/**
 * @file main.c
 * @brief 程序入口
 * @author hans
 * @version 0.1.0
 */
#include <common.h>

/**
 * 主函数
 * 传参只为配置文件，即：
 *   saws -c config.json
 * @param argc 参数个数
 * @param argv 参数列表
 * @return
 */
int main(int argc, const char *argv[]) {
  saws_log("Starting SAWS...");
  saws_err("An error occurred!");
  saws_warn("It's just a warning.");
  return 0;
}
