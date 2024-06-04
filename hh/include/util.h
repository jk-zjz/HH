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
#include "fiber.h"
/**
 * @util 工具函数的封装
 *
 * */
namespace hh {
    //当前线程
    pid_t GetThreadID();

    //当前携程
    uint32_t GetFiberID();

    //函数调用栈序列化
    void Backtrace(std::vector<std::string>& bt, int size=64, int skip =1);
    std::string BacktraceToString(int size=64, int skip=2, const std::string& prefix = "");
}
#endif //HH_UTIL_H
