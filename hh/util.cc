//
// Created by 35148 on 2024/4/30.
//
#include "util.h"
#include <sys/syscall.h>
#include <sstream>
#include <execinfo.h>


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

    /**
     * 获取程序的回溯信息。
     *
     * 该函数通过调用系统库backtrace和backtrace_symbols，获取指定数量的调用栈信息，并将其转换为可读的字符串形式，
     * 然后将这些信息添加到提供的字符串向量中。
     *
     * @param bt 引用传递，用于存储回溯信息的字符串向量。
     * @param size 预期收集的回溯信息条目数量。
     * @param skip 跳过的回溯信息条目数量，通常用于跳过当前函数等无用信息。
     */
    void Backtrace(std::vector<std::string>& bt, int size, int skip){
        // 分配内存以存储回溯数据。
        void **data = (void**)malloc(sizeof(void*) * size);;
        // 获取回溯信息。
        size_t s= ::backtrace(data, size);
        // 将二进制回溯数据转换为可读字符串。
        char **strings = backtrace_symbols(data, s);

        // 将转换后的回溯信息添加到向量中，跳过前skip个条目。
        for(size_t i = skip; i < s; ++i){
            bt.emplace_back(strings[i]);
        }

        // 释放动态分配的内存。
        free(strings);
        free(data);
    }
    /*
     * 序列化回溯信息
     * */
    std::string BacktraceToString(int size, int skip,const std::string& prefix){
        std::stringstream ss;
        std::vector<std::string> bt;
        Backtrace(bt, size, skip);
        for(size_t i=skip;i<bt.size();i++){
            ss << prefix << bt[i] << std::endl;
        }
        return ss.str();
    }
}
