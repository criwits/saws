/**
 * @file macro.h
 * @brief 各种宏定义
 * @author hans
 * @version 0.1
 */

#ifndef SAWS_MACRO_H
#define SAWS_MACRO_H

#define concat(a, b) a ## b
#define concat3(a, b, c) concat(concat(a, b), c)
#define str(a) # a

#define arrlen(x) (sizeof(x) / sizeof(x[0]))

#define likely(x)  __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

// already defined in param.h
// #define MAX(a, b) ((a > b) ? a : b)
// #define MIN(a, b) ((a < b) ? a : b)

#endif //SAWS_MACRO_H
