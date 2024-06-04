//
// Created by 35148 on 2024/5/23.
//
//协程模块开发
#ifndef HH_FIBER_H
#define HH_FIBER_H

#include "thread.h"
#include <ucontext.h>

namespace hh {
    class Fiber : public std::enable_shared_from_this<Fiber> {
    public:
        typedef std::shared_ptr<Fiber> ptr;
        enum State {
            INIT,   // 初始化状态
            HOLD,   // 暂停或等待状态
            EXEC,   // 执行状态
            TERM,   // 终止或结束状态
            READY,  // 准备就绪状态
            EXCEPT  // 异常或错误状态
        };
    private:
        //为创建主协程做的构造函数
        Fiber();
    public:
        //子协程的构造函数
       Fiber(std::function<void()> cb, size_t stackSize=0, bool useCaller=false);
        //协程的析构函数
        ~Fiber();
        /**
         * 协程结束，切换协程方法以便重新激活
         * */
        void reset(std::function<void()> cb);
        /**
         * 切换当前线程执行
         * */
        void swapIn();
        /**
         *切换到后台执行
         * */
        void swapOut();
        /**
         * 协程切换到执行
         * */
        void call();
        /**
         * 协程切换到后台执行
         * */
        void back();
        uint64_t getId() const { return m_id; }
    public:
        //设置当前协程
        static void SetThis(Fiber* f);
        // 获取当前正在执行的Fiber对象的智能指针
        static Fiber::ptr GetThis();
        // 获取当前系统中总的Fiber数量
        static uint64_t GetTotalFibers();
        // 将当前Fiber让出执行权，转到持有（HOLD）状态
        static void YieldToHold();
        // 将当前Fiber让出执行权，转到就绪（READY）状态等待下一次调度
        static void YieldToReady();
        //执行方法
        static void MainFunc();
        static uint32_t getFiber_id();
        //多协程调用主协程
        static void CallerMainFunc();
        State getState() const { return m_state; }
        void setState(State state) { m_state = state; }
    private:
        std::function<void()> m_cb;         //真正执行的函数
        ucontext_t m_ctx;                   //协程
        State m_state=INIT;                 //协程状态
        uint64_t m_id=0;                    //协程id
        uint32_t m_stackSize=0;              //协程栈空间
        void * m_stack=nullptr;               //协程栈空间
    };
}
#endif //HH_FIBER_H
