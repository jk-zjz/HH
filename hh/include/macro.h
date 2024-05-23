//
// Created by 35148 on 2024/5/23.
//

#ifndef HH_MACRO_H
#define HH_MACRO_H
#include <cstring>
#include "util.h"
#include "log.h"
#include <assert.h>
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
#endif //HH_MACRO_H
