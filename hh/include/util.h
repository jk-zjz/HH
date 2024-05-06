//
// Created by 35148 on 2024/4/30.
//

#ifndef HH_UTIL_H
#define HH_UTIL_H

#include <unistd.h>
#include <cstdint>
#include <thread>
namespace hh {
    //线程
    pid_t GetThreadID();

    //携程
    uint32_t GetFiberID();
}
#endif //HH_UTIL_H
