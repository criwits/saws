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

#endif //SAWS_MACRO_H
