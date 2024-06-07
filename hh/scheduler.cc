//
// Created by 35148 on 2024/5/29.
//
#include "scheduler.h"
#include "macro.h"

namespace hh {
    static Logger::ptr g_logger = HH_LOG_NAME("system");

    //协程调度器
    static thread_local Scheduler *t_scheduler = nullptr;
    //当前协程
    static thread_local Fiber *t_fiber = nullptr;
    /**
     * @scheduler 构造函数 线程数 是否使用调度线程 调度器名称
     * 如果启用调度线程会创建线程池--
     * */
    Scheduler::Scheduler(uint32_t threads, bool use_caller, std::string name) :
            m_name(name) {
        //创建线程数量>0
        HH_ASSERT(threads > 0);
        //需不需要使用一个线程做为调度线程
        if (use_caller) {
            //创建线程中的主协程
            hh::Fiber::GetThis();
            //线程数量减一
            --threads;
            //没有协程调度器
            HH_ASSERT(GetThis() == nullptr);
            t_scheduler = this;
            //创建主协程
            m_root_fiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
            hh::Thread::SetName(m_name);
            t_fiber = m_root_fiber.get();
            m_root_thread = hh::GetThreadID();
            m_thread_ids.push_back(m_root_thread);
        } else {
            m_root_thread = -1;
        }
        //线程池的真实大小
        m_thread_count = threads;
    }

    Scheduler::~Scheduler() {
        HH_ASSERT(m_stopping);
        if (GetThis() == this) {
            t_scheduler = nullptr;
        }
    }

    /**
     * @scheduler 启动调度器
     * 创建线程并且存储
     * */
    void Scheduler::start() {
        MutexType lock(m_mutex);
        if (!m_stopping) {
            //需要启动需要判断是否是停止状态才能启动
            return;
        }
        //已经启动
        m_stopping = false;
        //判断线程池是否为空，线程池为空则创建线程池
        HH_ASSERT(m_threads.empty());
        m_threads.resize(m_thread_count);
        for (size_t i = 0; i < m_thread_count; ++i) {
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                          m_name + "_" + std::to_string(i)));
            m_thread_ids.push_back(m_threads[i]->get_id());
        }
    }

    /**
     * @scheduler 停止调度器
     *
     * */
    void Scheduler::stop() {
        //设置需要自动停止标志
        m_auto_stop = true;
        if (m_root_fiber && m_thread_count == 0 &&
            (m_root_fiber->getState() == Fiber::State::TERM ||
             m_root_fiber->getState() == Fiber::State::INIT)) {
            //主线程已经创建 && 主协程已经结束 &&（主协程是停止状态 || 主协程是初始状态）
            m_stopping = true;
            //为用户的操作-是一个虚函数
            if (stopping()) {
                return;
            }
        }

        //是否有主线程
        if (m_root_thread != -1) {
            HH_ASSERT(GetThis() == this)
        } else {
            HH_ASSERT(GetThis() != this)
        }
        //停止标志
        m_stopping = true;
        for (size_t i = 0; i < m_thread_count; ++i) {
            //唤醒所有线程停止
            tickle();
        }
        if (m_root_thread) {
            //停止主线程
            tickle();
        }
        /**
         * 有主协程 && 主线程没有停止
         * */
        if (m_root_fiber && !stopping()) {
            m_root_fiber->call();
        }
        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }
        /**
         * 等待所有线程结束
         * */
        for (auto &i: thrs) {
            i->join();
        }
    }

    Scheduler *Scheduler::GetThis() {
        return t_scheduler;
    }

    Fiber *Scheduler::GetMainFiber() {
        return t_fiber;
    }

    /**
     * @scheduler 运行调度器
     * while(true)
     * 一直取任务 如果没有任务 就使用idle 任务空转
     * 判断是否为携程任务还是fun
     *
     * */
    void Scheduler::run() {
        //设置为当前执行的调度器
        setThis();
        //判断当前线程是否为主线程
        if (hh::GetThreadID() != m_root_thread) {
            //设置当前线程的协程
            t_fiber = Fiber::GetThis().get();
        }
        //设置空闲协程执行
        Fiber::ptr idle_fiber = std::make_shared<Fiber>(std::bind(&Scheduler::idle, this));
        //未了提供给fun函数，变成协程
        Fiber::ptr cb_fiber;

        //取任务
        FiberAndThread ft;
        while (true) {
            ft.reset();
            //需要唤醒线程
            bool tickle_me = false;
            bool is_active = false;
            {
                MutexType lock(m_mutex);
                while (!m_fibers.empty()) {
                    auto it = m_fibers.front();
                    m_fibers.pop();
                    if (it.thread_id != (uint32_t) -1 && it.thread_id != (uint32_t) hh::GetThreadID()) {
                        tickle_me = true;
                        m_fibers.push(it);
                        continue;
                    }

                    HH_ASSERT(it.fiber || it.function);
                    if (it.fiber && it.fiber->getState() == Fiber::EXEC) {
                        continue;
                    }
                    ft = it;
                    ++m_active_thread_count;
                    is_active = true;
                    break;
                }
                tickle_me |= m_fibers.empty();
            }
            //任务已经取出获取没有任务
            if (tickle_me) {
                //指定了执行协程，去重新唤醒
                tickle();
            }
            //是协程任务，并且不是停止态
            if (ft.fiber && (ft.fiber->getState() != Fiber::State::TERM &&
                             ft.fiber->getState() != Fiber::EXCEPT)) {

                //切换执行
                ft.fiber->swapIn();
                HH_LOG_INFO(g_logger, "schedule fiber run");
                --m_active_thread_count;
                if (ft.fiber->getState() == Fiber::State::READY) {
                    //这一个加入队列
                    schedule(ft.fiber);
                } else if (ft.fiber->getState() != Fiber::TERM &&
                           ft.fiber->getState() != Fiber::State::EXCEPT) {
                    //不是停止态或者异常 被打断
                    ft.fiber->setState(Fiber::State::HOLD);
                } else {
                    ft.reset();
                }
            } else if (ft.function) {
                if(cb_fiber) {
                    cb_fiber->reset(ft.function);
                } else {
                    cb_fiber.reset(new Fiber(ft.function));
                }
                ft.reset();
                cb_fiber->swapIn();
                --m_active_thread_count;
                if(cb_fiber->getState() == Fiber::READY) {
                    schedule(cb_fiber);
                    cb_fiber.reset();
                } else if(cb_fiber->getState() == Fiber::EXCEPT
                          || cb_fiber->getState() == Fiber::TERM) {
                    cb_fiber->reset(nullptr);
                } else {//if(cb_fiber->getState() != Fiber::TERM) {
                    cb_fiber->setState(Fiber::State::HOLD);
                    cb_fiber.reset();
                }
            } else {
                if (is_active) {
                    --m_active_thread_count;
                    continue;
                }
                //是一个空闲协程
                if (idle_fiber->getState() == Fiber::State::TERM) {
                    break;
                }
                ++m_idle_thread_count;
                idle_fiber->swapIn();
                --m_idle_thread_count;
                if (idle_fiber->getState() != Fiber::State::TERM
                    && idle_fiber->getState() != Fiber::State::EXCEPT) {
                    idle_fiber->setState(Fiber::State::HOLD);
                }
            }
        }
    }

    void Scheduler::setThis() {
        t_scheduler = this;
    }

    void Scheduler::tickle() {
        HH_LOG_INFO(g_logger, "tickle");
    }

    bool Scheduler::stopping() {
        /**
         * 是停止状态
         * 是要停止
         * 任务列表为空
         * 活跃线程为0
         * */
        HH_LOG_INFO(g_logger, "stopping");
        MutexType::Lock lock(m_mutex);
        return m_stopping &&
               m_auto_stop &&
               m_fibers.empty() &&
               m_active_thread_count == 0;
    }

    /**
     * @brief 空转任务
     *
     * */
    void Scheduler::idle() {
        HH_LOG_INFO(g_logger, "idle");
        while (!stopping()) {
            hh::Fiber::YieldToHold();
        }
    }

    /**
     * @scheduler 切换到其他线程执行
     * */
    void Scheduler::switchTo(int thread) {
        HH_ASSERT(Scheduler::GetThis() != nullptr);
        if (Scheduler::GetThis() == this) {
            if (thread == -1 || thread == hh::GetThreadID()) {
                return;
            }
        }
        //添加携程指定线程任务
        schedule(Fiber::GetThis(), thread);
        //切换到其他携程执行
        Fiber::YieldToHold();
    }

    std::ostream &Scheduler::dump(std::ostream &os) {
        os << "[Scheduler name=" << m_name
           << " size=" << m_thread_count
           << " active_count=" << m_active_thread_count
           << " idle_count=" << m_idle_thread_count
           << " stopping=" << m_stopping
           << " ]" << std::endl << "    ";
        for (size_t i = 0; i < m_threads.size(); ++i) {
            if (i) {
                os << ", ";
            }
            os << m_threads[i];
        }
        return os;
    }
}