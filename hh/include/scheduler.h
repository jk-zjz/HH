//
// Created by 35148 on 2024/5/29.
//

#ifndef HH_SCHEDULER_H
#define HH_SCHEDULER_H

#include "thread.h"
#include "fiber.h"
#include <memory>
#include <list>
#include <utility>

namespace hh {
    class Scheduler {
    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef Mutex MutexType;

        Scheduler(uint32_t threads = 1, bool use_caller = true, std::string name = "");

        virtual ~Scheduler();

        void start();   //启动调度器
        void stop();    //停止调度器

        static Scheduler *GetThis();    //获取当前线程的调度器
        static Fiber *GetFiber();       //获取当前线程的协程
        const std::string &getName() const { return m_name; };

        template<class FiberOrCb>
        void schedule(FiberOrCb fc, uint32_t thread = -1) {
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                need_tickle = scheduleNoLook(fc, thread);
            }
            if(need_tickle){
                tickle();
            }
        }

        template<class InputIterator>
        void schedule(InputIterator begin, InputIterator end){
            bool need_tickle = m_fibers.empty();
            {
                MutexType::Lock lock(m_mutex);
                while(begin!=end){
                    need_tickle = scheduleNoLook(*begin)||need_tickle;
                    ++begin;
                }
            }
            if(need_tickle){
                tickle();
            }
        }
    protected:
        virtual void tickle();
    private:
        template<class FiberOrCb>
        bool scheduleNoLook(FiberOrCb fc, uint32_t thread) {
            bool need_tickle = m_fibers.empty();
            FiberAndThread ft(fc, thread);
            if (ft.function || ft.fiber) {
                m_fibers.push_back(ft);
            }
            return need_tickle;
        }

    private:
        struct FiberAndThread {
            Fiber::ptr fiber;               //协程
            std::function<void()> function;  //函数
            uint32_t thread_id;             //线程id
            FiberAndThread(Fiber::ptr f, uint32_t t) :
                    fiber(std::move(f)), thread_id(t) {
            }

            FiberAndThread(std::function<void()> f, uint32_t t) :
                    function(std::move(f)), thread_id(t) {
            }

            FiberAndThread(Fiber::ptr *f, uint32_t t) : thread_id(t) {
                fiber.swap(*f);
            }

            FiberAndThread(std::function<void()> *f, uint32_t t) : thread_id(t) {
                function.swap(*f);
            }

            FiberAndThread() : thread_id(-1) {
            }

            void reset() {
                fiber = nullptr;
                function = nullptr;
                thread_id = -1;
            }
        };

    private:
        std::vector<Thread> m_threads;                  //线程组
        std::string m_name;                             //线程池名称
        MutexType m_mutex;                              //互斥锁
        std::list<FiberAndThread> m_fibers;             //协程获方法队列
    };
}
#endif //HH_SCHEDULER_H
