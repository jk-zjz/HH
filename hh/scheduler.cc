//
// Created by 35148 on 2024/5/29.
//
#include "scheduler.h"

#include <utility>
#include "log.h"
#include "macro.h"

namespace hh{
    static Logger::ptr g_logger = HH_LOG_NAME("system");
    //协程调度器
    static thread_local Scheduler* t_scheduler = nullptr;
    //可以执行的主协程
    static thread_local Fiber* t_fiber = nullptr;
    Scheduler::Scheduler(uint32_t threads, bool use_caller, std::string name):
    m_name(std::move(name)){
        HH_ASSERT(threads > 0);
        if(use_caller){
            --threads;
            //创建协程调度主协程
            hh::Fiber::GetThis();
            //判断是否有执行主协程
            HH_ASSERT(GetThis() == nullptr);
            t_scheduler = this;
            //创建主协程，为调度器的主协程，绑定run方法，上面主协程绑定的是协程中的主协程的fun方法
            m_root_fiber.reset(new Fiber(std::bind(&Scheduler::run, this)));
            m_root_thread=hh::GetThreadID();
            t_fiber = m_root_fiber.get();
            m_thread_ids.push_back(m_root_thread);
        }else{
            m_root_thread = -1;
        }
        m_thread_count = threads;
    }

    Scheduler::~Scheduler() {
        HH_ASSERT(m_stopping);
        if(GetThis() == this){
            t_scheduler = nullptr;
        }
    }

    void Scheduler::start() {
        MutexType lock(m_mutex);
        if(!m_stopping){
            return;
        }
        m_stopping=false;
        HH_ASSERT(m_threads.empty());
        m_threads.resize(m_thread_count);
        for(int i=0;i<m_thread_count;++i){
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                          m_name+"_"+std::to_string(i)));
            m_thread_ids.push_back(m_threads[i]->get_id());
        }

    }

    void Scheduler::stop() {
        //设置需要自动停止标志
        m_auto_stop = true;
        if(m_root_fiber && m_thread_count==0 &&
                (m_root_fiber->getState() == Fiber::State::TERM ||
                m_root_fiber->getState()==Fiber::State::INIT)){
            //主线程已经创建 && 主协程已经结束 &&（主协程是停止状态 || 主协程是初始状态）
            m_stopping = true;
            //为用户的操作-是一个虚函数
            if(stopping()){
                return;
            }
        }
        bool need_tickle = false;
        //是否有主线程
        if(m_root_thread != -1){
            HH_ASSERT(GetThis() == this)
        }else{
            HH_ASSERT(GetThis() != this)
        }
        //停止标志
        m_stopping = true;
        for(int i=0;i<m_threads.size();++i){
            //唤醒所有线程停止
            tickle();
        }
        if(m_root_thread){
            //停止主线程
            tickle();
        }
    }

    Scheduler *Scheduler::GetThis() {
        return t_scheduler;
    }

    Fiber *Scheduler::GetFiber() {
        return t_fiber;
    }

    void Scheduler::run() {
        //设置为当前执行的调度器
        setThis();
        //判断当前线程是否为主线程
        if(hh::GetThreadID() != m_root_thread){
            //设置当前线程的协程
            t_fiber = Fiber::GetThis().get();
        }
        //设置空闲协程执行
        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
        //未了提供给fun函数，变成协程
        Fiber::ptr cb_fiber;

        //取任务
        FiberAndThread ft;
        while(true){
            ft.reset();
            //需要唤醒线程
            bool tickle_me = false;
            {
                MutexType lock(m_mutex);
                auto it = m_fibers.begin();
                //取任务
                while (it!=m_fibers.end()){
                    //当前任务需要指定线程执行，并且不是当前线程
                    if(it->thread_id!=-1 && it->thread_id!=hh::GetThreadID()){
                        ++it;
                        //需要唤醒线程
                        tickle_me = true;
                        continue;
                    }
                    HH_ASSERT(it->fiber || it->function);
                    //是一个协程并且正在运行，那直接不管
                    if(it->fiber && it->fiber->getState() == Fiber::State::EXEC){
                        //当前协程正在执行
                        ++it;
                        continue;
                    }
                    //取出任务
                    ft = *it;
                    m_fibers.erase(it);
                    break;
                }
            }
            //任务已经取出获取没有任务
            if(tickle_me){
                //指定了执行协程，去重新唤醒
                tickle();
            }
            //是协程任务，并且不是停止态
            if(ft.fiber && ft.fiber->getState()!=Fiber::State::TERM){
                //是一个协程--运行协程数量++
                ++m_active_thread_count;
                //切换执行
                ft.fiber->swapIn();
                --m_active_thread_count;
                if(ft.fiber->getState() == Fiber::State::READY){
                    //这一个加入队列
                    schedule(ft.fiber);
                }else if(ft.fiber->getState()!=Fiber::TERM ||
                            ft.fiber->getState()!=Fiber::State::EXCEPT){
                    //不是停止态或者异常 被打断
                    ft.fiber->setState(Fiber::State::HOLD);
                }else{
                    //我觉得是这样的------------------------------------------------------------------------
                    //ft.reset();
                }
                ft.reset();
            }else if(ft.function){
                //是一个函数
                if(cb_fiber){
                    //有空闲协程
                    cb_fiber->reset(ft.function);
                }else{
                    //没有空闲协程
                    cb_fiber.reset(new Fiber(ft.function));
                }
                ft.reset();
                ++m_active_thread_count;
                cb_fiber->swapIn();
                --m_active_thread_count;
                if(cb_fiber->getState()==Fiber::READY){
                    //准备就绪状态
                    schedule(cb_fiber);
                }else if(cb_fiber->getState()==Fiber::TERM
                    || cb_fiber->getState()==Fiber::State::EXCEPT){
                    //执行完毕
                    cb_fiber->reset(nullptr);
                }else{
                    cb_fiber->setState(Fiber::State::HOLD);
                    cb_fiber.reset();
                }
            }else{
                //是一个空闲协程
                if (idle_fiber->getState()==Fiber::State::TERM){
                    break;
                }
                ++m_idle_thread_count;
                idle_fiber->swapIn();
                --m_idle_thread_count;
                if(idle_fiber->getState()!=Fiber::State::TERM
                        && idle_fiber->getState()!=Fiber::State::EXCEPT){
                    idle_fiber->setState(Fiber::State::HOLD);
                }
            }
        }
    }

    void Scheduler::setThis() {
        t_scheduler = this;
    }

}