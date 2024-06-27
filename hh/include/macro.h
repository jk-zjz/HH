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
#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define HH_LIKELY(x)       __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#   define HH_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define HH_LIKELY(x)      (x)
#   define HH_UNLIKELY(x)      (x)
#endif
#define HH_ASSERT(x) \
    if(HH_UNLIKELY(!(x))){        \
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"ASSERTION: " #x \
        <<"\nBacktraceToString\n"                                        \
        <<hh::BacktraceToString(100,2,"    ");                           \
        assert(x);                 \
    }
#define HH_ASSERT2(x,y) \
    if(HH_UNLIKELY(!(x))){        \
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"ASSERTION: " #x \
        <<"\n"<<y                \
        <<"\nBacktraceToString\n"                                        \
        <<hh::BacktraceToString(100,2,"    ");                           \
        assert(x);                 \
    }
#   define HH_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#endif //HH_MACRO_H
