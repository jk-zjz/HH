//
// Created by 35148 on 2024/5/29.
//

#ifndef HH_SCHEDULER_H
#define HH_SCHEDULER_H

#include <memory>
#include <list>
#include <utility>

#include "thread.h"

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
        static Fiber *GetMainFiber();       //获取当前线程的协程
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
                    need_tickle = scheduleNoLook(&*begin,-1)||need_tickle;
                    ++begin;
                }
            }
            if(need_tickle){
                tickle();
            }
        }
        void switchTo(int threadid =-1);
        std::ostream &dump(std::ostream &os);
    protected:
        virtual void tickle();
        void run();
        virtual bool stopping();
        void setThis();
        virtual void idle();
        bool hasIdleThreads(){return m_idle_thread_count>0;};
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
            Fiber::ptr fiber;                   //协程
            std::function<void()> function;     //函数
            uint32_t thread_id;                 //需要指定执行的线程id
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
        std::vector<Thread::ptr> m_threads;             //线程组
        std::string m_name;                             //线程池名称
        MutexType m_mutex;                              //互斥锁
        std::list<FiberAndThread> m_fibers;             //协程获方法队列
        Fiber::ptr m_root_fiber;                        //主协程
    protected:
        std::vector<int> m_thread_ids;                  //线程id
        bool m_stopping = true;                         //是否停止
        bool m_auto_stop =false;                        //是否自动停止
        uint32_t m_thread_count=0;                      //线程数量
        std::atomic<uint32_t> m_active_thread_count = {0};             //活跃线程数量
        std::atomic<uint32_t> m_idle_thread_count = {0};               //空闲线程数量
        int m_root_thread = 0;                          //主线程id

    };
}
#endif //HH_SCHEDULER_H
