//
// Created by 35148 on 2024/4/30.
//
#include "util.h"



namespace hh {
    pid_t GetThreadID() {
        return getpid();

    }

    uint32_t GetFiberID() {
        //现在没有协程暂时
        return 0;
    }

}
