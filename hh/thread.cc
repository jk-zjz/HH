//
// Created by 35148 on 2024/5/17.
//
#include "log.h"
#include <utility>
namespace hh{
    /*
     * 系统库统一日志system
     * */
    static std::string system_name = "system";
    static  hh::Logger::ptr g_logger = HH_LOG_NAME(system_name.c_str());
    /**
     * 在线程局部存储中定义一个Thread类型的指针和一个字符串，
     * 用于标识当前线程和未指定的线程名称。
     *
     * 这段代码不位于函数或类的内部，因此没有参数和返回值。
     * 它定义了两个线程局部变量：
     * - t_thread: 指向Thread类型的指针，用于表示当前线程。
     * - t_thread_name: 一个字符串，用于存储当前线程的名称，默认为"UNKNOWN"。
     * 两个静态变量标注为thread_local，它们只能在当前线程中访问，而不会跨线程共享。
     **/
    static thread_local Thread *t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOWN";
    Thread *Thread::GetThis() {
        return t_thread;
    }

    const std::string Thread::GetName() {
        return t_thread_name;
    }

    void Thread::SetName(const std::string &name) {
        if(t_thread){
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }
    Thread::Thread(std::function<void()> cb, const std::string &name):
    m_name(name)
    ,m_cb(std::move(cb)){
        if(name.empty()){
            m_name="UNKNOWN";
        }
        int re = pthread_create(&m_thread,
                                nullptr,
                                &Thread::run,
                                this);
        if(re){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
                        <<"pthread_create error "
                        <<re<<" name="<<name;
            throw std::logic_error("pthread_create error");
        }
        m_semaphore.wait();
    }

    hh::Thread::~Thread() {
        if(m_thread){
            pthread_detach(m_thread);
        }
    }

    void hh::Thread::join() {
        if(m_thread){
            int re = pthread_join(m_thread, nullptr);
            if(re){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
                        <<"pthread_join error "
                        <<re<<" name="<<m_name;
                throw std::logic_error("pthread_join error");
            }
        }
        m_thread = 0;
    }

    void *Thread::run(void *arg) {
        Thread *thread = (Thread *) arg;
        //设置全局线程变量
        t_thread= thread;
        t_thread_name=thread->m_name;
        //获取线程id
        thread->m_id=hh::GetThreadID();
        //更改线程名 只能接受15个字符
        pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
        std::function<void()>cx;
        cx.swap(thread->m_cb);
        t_thread->m_semaphore.notify();
        cx();
        return 0;
    }

    Semaphore::Semaphore(uint32_t count) {
        if(sem_init(&m_semaphore,0,count)){
            throw std::logic_error("sem_init error");
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
                    <<"sem_init error";
        }

    }

    Semaphore::~Semaphore() {
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait() {
        if(sem_wait(&m_semaphore)){
            throw std::logic_error("sem_wait error");
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
                    <<"sem_wait error";
        }
    }

    void Semaphore::notify() {
        if(sem_post(&m_semaphore)){
            throw std::logic_error("sem_post error");
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
                    <<"sem_post error";
        }
    }
}
