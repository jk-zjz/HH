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
namespace hh {
    class Semaphore {
    public:
        Semaphore(uint32_t count = 0);

        ~Semaphore();

        /**
         * @brief 等待函数，使当前线程进入等待状态。
         *
         * 该函数用于线程间的同步操作，当一个线程调用此函数时，它会进入等待状态，直到其他线程对其进行通知。
         * 该函数没有参数，也不返回任何值。
         */
        void wait();
        /**
         * @brief 通知函数，用于唤醒等待状态的线程。
         *
         * 当一个线程调用此函数时，它会尝试唤醒一个因调用wait()函数而处于等待状态的线程。
         * 该函数没有参数，也不返回任何值。
         */
        void notify();
    private:
        sem_t m_semaphore;
    private:
        Semaphore(const Semaphore &) = delete;

        Semaphore(const Semaphore &&) = delete;

        Semaphore operator=(const Semaphore &) = delete;
    };
    template<class T>
    struct ScopedLockImpl {
    public:
        //构造加锁---析构解锁
        ScopedLockImpl(T & mutex)
            :m_mutex(mutex){
            m_mutex.lock();
            m_locked = true;
        };
        ~ScopedLockImpl(){
            unlock();
        };
        void lock(){
            if(!m_locked){
                m_mutex.lock();
                m_locked = true;
            }
        }
        void unlock(){
            if(m_locked){
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        T & m_mutex;
        bool m_locked;
    };
    template<class T>
    struct ReadScopedLockImpl {
    public:
        //构造加锁---析构解锁
        ReadScopedLockImpl(T & mutex)
                :m_mutex(mutex){
            m_mutex.readLock();
            m_locked = true;
        };
        ~ReadScopedLockImpl(){
            unlock();
        };
        void lock(){
            if(!m_locked){
                m_mutex.readLock();
                m_locked = true;
            }
        }
        void unlock(){
            if(m_locked){
                m_mutex.unLock();
                m_locked = false;
            }
        }
    private:
        T & m_mutex;
        bool m_locked;
    };
    template<class T>
    struct WriteScopedLockImpl {
    public:
        //构造加锁---析构解锁
        WriteScopedLockImpl(T & mutex)
                :m_mutex(mutex){
            m_mutex.writeLock();
            m_locked = true;
        };
        ~WriteScopedLockImpl(){
            unlock();
        };
        void lock(){
            if(!m_locked){
                m_mutex.writeLock();
                m_locked = true;
            }
        }
        void unlock(){
            if(m_locked){
                m_mutex.unLock();
                m_locked = false;
            }
        }
    private:
        T & m_mutex;
        bool m_locked;
    };
    class SpinLock {
    public:
        typedef ScopedLockImpl<SpinLock> Lock;
        SpinLock(){
            pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);
        }
        ~SpinLock(){
            pthread_spin_destroy(&m_spinlock);
        }
        void lock(){
            pthread_spin_lock(&m_spinlock);
        }
        void unlock(){
            pthread_spin_unlock(&m_spinlock);
        }
    private:
        //自旋锁
        pthread_spinlock_t m_spinlock;
    };
    class RWMutex {
    public:
        typedef ReadScopedLockImpl<RWMutex> ReadLock;
        typedef WriteScopedLockImpl<RWMutex> WriteLock;
        RWMutex(){
            pthread_rwlock_init(&m_rwlock, nullptr);
        }
        ~RWMutex(){
            pthread_rwlock_destroy(&m_rwlock);
        }
        void readLock(){
            pthread_rwlock_rdlock(&m_rwlock);
        }
        void writeLock(){
            pthread_rwlock_wrlock(&m_rwlock);
        }
        void unLock(){
            pthread_rwlock_unlock(&m_rwlock);
        }
    private:
        pthread_rwlock_t m_rwlock;
    };
    class Mutex{
    public:
        typedef ScopedLockImpl<Mutex> Lock;
        Mutex(){
            pthread_mutex_init(&m_mutex, nullptr);
        }
        ~Mutex(){
            pthread_mutex_destroy(&m_mutex);
        }
        void lock(){
            pthread_mutex_lock(&m_mutex);
        }
        void unlock(){
            pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };
    class NullMutex{
    public:
        typedef ScopedLockImpl<NullMutex> Lock;
        NullMutex(){};
        ~NullMutex(){};
        void lock(){};
        void unlock(){};
    };
    class NullRWMutex {
    public:
        typedef ReadScopedLockImpl<NullMutex> ReadLock;
        typedef WriteScopedLockImpl<NullMutex> WriteLock;
        NullRWMutex(){};
        ~NullRWMutex(){};
        void readLock(){};
        void writeLock(){};
        void unLock(){};
    };
    /**
     * 类CASlock实现了一个基于原子操作的自旋锁。
     * 乐观锁
     */
    class CASlock{
    public:
        CASlock(CASlock& slock) {

        }

        typedef ScopedLockImpl<CASlock> Lock;
        /**
         * CASlock构造函数，初始化自旋锁。
         */
        CASlock(){
            m_flag.clear(); // 初始化原子标志为未锁定状态
        }

        /**
         * CASlock析构函数，无操作。
         */
        ~CASlock(){
        }

        /**
         * 尝试获取锁。
         * 该方法使用CAS（Compare-and-Swap）操作实现锁的获取，如果锁已被其他线程持有，则会自旋等待。
         */
        void lock(){
            while(std::atomic_flag_test_and_set_explicit(&m_flag, std::memory_order_acquire));
        }

        /**
         * 释放锁。
         */
        void unlock(){
            std::atomic_flag_clear_explicit(&m_flag, std::memory_order_release);
        }
    private:
        volatile std::atomic_flag m_flag; // 原子标志，用于实现自旋锁
    };
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
