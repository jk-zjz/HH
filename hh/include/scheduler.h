//
// Created by 35148 on 2024/5/29.
//

#ifndef HH_SCHEDULER_H
#define HH_SCHEDULER_H

#include <memory>

#include <utility>
#include "thread.h"


namespace hh {
    //携程调度器
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

        /**
         * @schedule 单添加任务方法
         *
         * */
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
        /**
         * @schedule 批量添加任务方法
         *
         * */
        template<class InputIterator>
        void schedule(InputIterator begin, InputIterator end){
            bool need_tickle = false;
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
        virtual void idle();
        //唤醒
        virtual void tickle();
        //线程运行函数
        void run();
        //判断停止
        virtual bool stopping();

        void setThis();

        //空闲线程
        bool hasIdleThreads(){return m_idle_thread_count>0;};
    private:
        /**
         * @schedule 添加任务方法
         * */
        template<class FiberOrCb>
        bool scheduleNoLook(FiberOrCb fc, uint32_t thread) {
            bool need_tickle = m_fibers.empty();
            FiberAndThread ft(fc, thread);
            if (ft.function || ft.fiber ) {
                m_fibers.push_back(ft);
            }
            return need_tickle;
        }

    private:
        /**
         * 任务结构体
         * */
        struct FiberAndThread {
            Fiber::ptr fiber;                   //协程
            std::function<void()> function;     //函数
            uint32_t thread_id;                 //需要指定执行的线程id
            bool Master_NULL(){
                return fiber== nullptr||function== nullptr;
            };
            bool Master_coroutine(){
                return fiber != nullptr||function!= nullptr;
            };
            FiberAndThread(Fiber::ptr f, int thr)
                    :fiber(f), thread_id(thr) {
            }
            /**
             * @brief 构造函数
             * @param[in] f 协程指针
             * @param[in] thr 线程id
             * @post *f = nullptr
             */
            FiberAndThread(Fiber::ptr* f, int thr)
                    :thread_id(thr) {
                fiber.swap(*f);
            }

            /**
             * @brief 构造函数
             * @param[in] f 协程执行函数
             * @param[in] thr 线程id
             */
            FiberAndThread(std::function<void()> f, int thr)
                    :function(f), thread_id(thr) {
            }

            /**
             * @brief 构造函数
             * @param[in] f 协程执行函数指针
             * @param[in] thr 线程id
             * @post *f = nullptr
             */
            FiberAndThread(std::function<void()>* f, int thr)
                    :thread_id(thr) {
                function.swap(*f);
            }

            /**
             * @brief 无参构造函数
             */
            FiberAndThread()
                    :thread_id(-1) {
            }

            /**
             * @brief 重置数据
             */
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
        std::vector<FiberAndThread> m_fibers;             //协程获方法队列
        Fiber::ptr m_root_fiber;                        //主协程
    protected:
        std::vector<int> m_thread_ids;                  //线程id
        bool m_stopping = true;                         //是否停止
        bool m_auto_stop =false;                        //是否自动停止
        uint32_t m_thread_count=0;                      //线程数量
        std::atomic<uint32_t> m_active_thread_count = {0};               //活跃线程数量
        std::atomic<uint32_t> m_idle_thread_count = {0};                 //空闲线程数量
        int m_root_thread = 0;                          //主线程id

    };
}
#endif //HH_SCHEDULER_H
