//
// Created by 35148 on 2024/4/30.
//
#include "util.h"
#include <sys/syscall.h>


namespace hh {
    pid_t GetThreadID() {
        //getpid 返回的是进程id
        //return getpid();
        //这个返回的是线程id
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberID() {
        //现在没有协程暂时
        return 0;
    }

}
