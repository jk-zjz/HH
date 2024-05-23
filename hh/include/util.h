//
// Created by 35148 on 2024/4/30.
//

#ifndef HH_UTIL_H
#define HH_UTIL_H

#include <unistd.h>
#include <cstdint>
#include <thread>
#include <vector>
#include <string>
namespace hh {
    //线程
    pid_t GetThreadID();

    //携程
    uint32_t GetFiberID();

    //函数调用栈序列化
    void Backtrace(std::vector<std::string>& bt, int size, int skip);
    std::string BacktraceToString(int size, int skip=2, const std::string& prefix = "");
}
#endif //HH_UTIL_H
