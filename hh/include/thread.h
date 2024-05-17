//
// Created by 35148 on 2024/5/17.
//
#ifndef HH_THREAD_H
#define HH_THREAD_H
#include <thread>
#include <functional>
#include <pthread.h>
#include <string>
#include <memory>
#include "log.h"
#include "util.h"
namespace hh{
class Thread{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb,const std::string& name);
    ~Thread();

    pid_t get_id()const{return m_id;}
    const std::string& get_name()const{return m_name;}

    void join();

    static Thread* GetThis();
    static const std::string GetName();
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&)=delete;
    Thread& operator=(const Thread&) = delete;
    static void* run(void *arg);
private:
    /**
     * 类成员变量说明：
     *
     * m_name - 成员变量名称，类型为std::string，用于存储名称。
     * m_id - 成员变量ID，类型为pid_t，用于存储进程ID。
     * m_thread - 成员变量线程标识符，类型为pthread_t，用于标识线程。
     * m_cb - 成员变量回调函数，类型为std::function<void()>，用于存储一个无参数的回调函数。
     */
    std::string m_name;
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
};
}
#endif //HH_THREAD_H
