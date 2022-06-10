/**
 * @file logger.h
 * @brief 此头文件定义了用来打印日志的一些宏。
 * @author hans
 * @version 0.1.0
 */
#ifndef SAWS_LOGGER_H
#define SAWS_LOGGER_H

#include <stdio.h>
#include <time.h>

#define SAWS_DEBUG_ENABLED

/*** ANSI 颜色代码 ***/

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

/**
 * 此宏用来打印指定颜色的一整行内容。
 * 语法大致同 printf。
 * @example saws_formatted_log(ANSI_FG_BLUE, "Hi, my name is %s", "Hans"); 可以打印一行蓝色的“Hi, my name is Hans”。
 */
#define saws_formatted_log(colour, format, ...) \
  printf(colour "[%s:%d %s] " format ANSI_NONE "\n", \
  __FILE__, __LINE__, __func__, ## __VA_ARGS__)

/**
 * 下面的三个宏用来打印“日志”（蓝色）、“警告”（黄色）和“错误”（红色）。
 * 本质就是对上面那个宏的固定化调用。
 */
#define saws_log(format, ...) \
  saws_formatted_log(ANSI_FG_BLUE, format, ## __VA_ARGS__)

#define saws_warn(format, ...) \
  saws_formatted_log(ANSI_FG_YELLOW, format, ## __VA_ARGS__)

#define saws_err(format, ...) \
  saws_formatted_log(ANSI_FG_RED, format, ## __VA_ARGS__)

#ifdef SAWS_DEBUG_ENABLED
#define saws_debug(format, ...) \
  do { \
    time_t rawtime; \
    struct tm *ptminfo; \
    time(&rawtime); \
    ptminfo = localtime(&rawtime); \
    printf(ANSI_FG_MAGENTA "[%02d-%02d-%02d %02d:%02d:%02d] " ANSI_FG_CYAN format ANSI_NONE "\n", \
      ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday, ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec, ## __VA_ARGS__); \
  } while(0) \

#define saws_debug_room(format, room_id, ...) \
  saws_debug(ANSI_FG_GREEN "\b[Room %d] " ANSI_FG_CYAN format, room_id, ## __VA_ARGS__)

#else
#define saws_debug(format, ...)
#define saws_debug_room(room_id, format, ...)
#endif

#endif //SAWS_LOGGER_H
