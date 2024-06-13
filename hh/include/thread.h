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
#include "util.h"
#include <semaphore.h>
#include <iostream>
#include <atomic>
#include "Noncopyable.h"
namespace hh {
    class Semaphore : Noncopyable {
    public:
        /**
         * @brief 构造函数
         * @param[in] count 信号量值的大小
         */
        Semaphore(uint32_t count = 0);

        /**
         * @brief 析构函数
         */
        ~Semaphore();

        /**
         * @brief 获取信号量
         */
        void wait();

        /**
         * @brief 释放信号量
         */
        void notify();
    private:
        sem_t m_semaphore;
    };

/**
 * @brief 局部锁的模板实现
 */
    template<class T>
    struct ScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex Mutex
         */
        ScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
            m_mutex.lock();
            m_locked = true;
        }

        /**
         * @brief 析构函数,自动释放锁
         */
        ~ScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 加锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.lock();
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

/**
 * @brief 局部读锁模板实现
 */
    template<class T>
    struct ReadScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        ReadScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
            m_mutex.rdlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数,自动释放锁
         */
        ~ReadScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 上读锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.rdlock();
                m_locked = true;
            }
        }

        /**
         * @brief 释放锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

/**
 * @brief 局部写锁模板实现
 */
    template<class T>
    struct WriteScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        WriteScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
            m_mutex.wrlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数
         */
        ~WriteScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 上写锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.wrlock();
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// Mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

/**
 * @brief 互斥量
 */
    class Mutex : Noncopyable {
    public:
        /// 局部锁
        typedef ScopedLockImpl<Mutex> Lock;

        /**
         * @brief 构造函数
         */
        Mutex() {
            pthread_mutex_init(&m_mutex, nullptr);
        }

        /**
         * @brief 析构函数
         */
        ~Mutex() {
            pthread_mutex_destroy(&m_mutex);
        }

        /**
         * @brief 加锁
         */
        void lock() {
            pthread_mutex_lock(&m_mutex);
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            pthread_mutex_unlock(&m_mutex);
        }
    private:
        /// mutex
        pthread_mutex_t m_mutex;
    };

/**
 * @brief 空锁(用于调试)
 */
    class NullMutex : Noncopyable{
    public:
        /// 局部锁
        typedef ScopedLockImpl<NullMutex> Lock;

        /**
         * @brief 构造函数
         */
        NullMutex() {}

        /**
         * @brief 析构函数
         */
        ~NullMutex() {}

        /**
         * @brief 加锁
         */
        void lock() {}

        /**
         * @brief 解锁
         */
        void unlock() {}
    };

/**
 * @brief 读写互斥量
 */
    class RWMutex : Noncopyable{
    public:

        /// 局部读锁
        typedef ReadScopedLockImpl<RWMutex> ReadLock;

        /// 局部写锁
        typedef WriteScopedLockImpl<RWMutex> WriteLock;

        /**
         * @brief 构造函数
         */
        RWMutex() {
            pthread_rwlock_init(&m_lock, nullptr);
        }

        /**
         * @brief 析构函数
         */
        ~RWMutex() {
            pthread_rwlock_destroy(&m_lock);
        }

        /**
         * @brief 上读锁
         */
        void rdlock() {
            pthread_rwlock_rdlock(&m_lock);
        }

        /**
         * @brief 上写锁
         */
        void wrlock() {
            pthread_rwlock_wrlock(&m_lock);
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            pthread_rwlock_unlock(&m_lock);
        }
    private:
        /// 读写锁
        pthread_rwlock_t m_lock;
    };

/**
 * @brief 空读写锁(用于调试)
 */
    class NullRWMutex : Noncopyable {
    public:
        /// 局部读锁
        typedef ReadScopedLockImpl<NullMutex> ReadLock;
        /// 局部写锁
        typedef WriteScopedLockImpl<NullMutex> WriteLock;

        /**
         * @brief 构造函数
         */
        NullRWMutex() {}
        /**
         * @brief 析构函数
         */
        ~NullRWMutex() {}

        /**
         * @brief 上读锁
         */
        void rdlock() {}

        /**
         * @brief 上写锁
         */
        void wrlock() {}
        /**
         * @brief 解锁
         */
        void unlock() {}
    };

/**
 * @brief 自旋锁
 */
    class Spinlock : Noncopyable {
    public:
        /// 局部锁
        typedef ScopedLockImpl<Spinlock> Lock;

        /**
         * @brief 构造函数
         */
        Spinlock() {
            pthread_spin_init(&m_mutex, 0);
        }

        /**
         * @brief 析构函数
         */
        ~Spinlock() {
            pthread_spin_destroy(&m_mutex);
        }

        /**
         * @brief 上锁
         */
        void lock() {
            pthread_spin_lock(&m_mutex);
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            pthread_spin_unlock(&m_mutex);
        }
    private:
        /// 自旋锁
        pthread_spinlock_t m_mutex;
    };

/**
 * @brief 原子锁
 */
    class CASLock : Noncopyable {
    public:
        /// 局部锁
        typedef ScopedLockImpl<CASLock> Lock;

        /**
         * @brief 构造函数
         */
        CASLock() {
            m_mutex.clear();
        }

        /**
         * @brief 析构函数
         */
        ~CASLock() {
        }

        /**
         * @brief 上锁
         */
        void lock() {
            while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
        }
    private:
        /// 原子状态
        volatile std::atomic_flag m_mutex;
    };


    /**
     * @thread 线程类封装
     *
     * */
    class Thread {
    public:

        typedef std::shared_ptr<Thread> ptr;

        Thread(std::function<void()> cb, const std::string &name);

        ~Thread();

        pid_t get_id() const { return m_id; }

        const std::string &get_name() const { return m_name; }

        //线程阻塞等待
        void join();

        //获取当前线程信息
        static Thread *GetThis();

        static const std::string GetName();

        static void SetName(const std::string &name);

    private:
        Thread(const Thread &) = delete;

        Thread(const Thread &&) = delete;

        Thread &operator=(const Thread &) = delete;

        //线程运行函数
        static void *run(void *arg);

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

        Semaphore m_semaphore;
    };
}
#endif //HH_THREAD_H
