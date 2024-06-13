//
// Created by 35148 on 2024/5/23.
//

#ifndef HH_MACRO_H
#define HH_MACRO_H
#include <cstring>
#include "util.h"
#include "log.h"
#include <cassert>
/**
 * @macro 断言&函数栈的宏设置
 *
 *
 * */
#define HH_ASSERT(x) \
    if(!(x)){        \
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"ASSERTION: " #x \
        <<"\nBacktraceToString\n"                                        \
        <<hh::BacktraceToString(100,2,"    ");                           \
        assert(x);                 \
    }
#define HH_ASSERT2(x,y) \
    if(!(x)){        \
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"ASSERTION: " #x \
        <<"\n"<<y                \
        <<"\nBacktraceToString\n"                                        \
        <<hh::BacktraceToString(100,2,"    ");                           \
        assert(x);                 \
    }
#   define HH_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#endif //HH_MACRO_H
