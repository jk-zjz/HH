//
// Created by 35148 on 2024/5/23.
//
#include "fiber.h"
#include <atomic>
#include <utility>
#include "config.h"
#include "macro.h"
namespace hh {
    //全局原子计数器
    static std::atomic<uint32_t> s_fiber_id{0};
    static std::atomic<uint32_t> s_fiber_count{0};

    //当前线程
    static thread_local Fiber *t_fiber = nullptr;
    //master fiber
    static thread_local Fiber::ptr t_threadFiber = nullptr;

    //注册fiber栈大小配置
    static hh::ConfigVar<uint32_t>::ptr g_fiber_stack_size = hh::Config::Lookup<uint32_t>("fiber.stack_size",
                                                                                          1024 * 1024, "stack size");
    class MallocStackAllocator {
    public:
        static void *Alloc(size_t size) {
            return malloc(size);
        }

        static void Dealloc(void *ptr, size_t size) {
            free(ptr);
        }
    };

    using StackAllocator = MallocStackAllocator;

    Fiber::Fiber() {
        m_state = EXEC;
        SetThis(this);
        if (setcontext(&m_ctx)) {
            HH_ASSERT2(false, "Fiber::Fiber");
        }
        s_fiber_count++;
    }

    Fiber::Fiber(std::function<void()> cb, size_t stackSize) :
            m_cb(std::move(cb)),
            m_id(++s_fiber_id){
        s_fiber_count++;
        m_stackSize=stackSize ?stackSize:g_fiber_stack_size->getValue();
        m_stack = (char *)StackAllocator::Alloc(m_stackSize);
        if (getcontext(&m_ctx)) {
            HH_ASSERT2(false, "Fiber::Fiber");
        }
        m_ctx.uc_link = nullptr;
        //指定栈空间
        m_ctx.uc_stack.ss_sp = m_stack;
        //指定栈大小
        m_ctx.uc_stack.ss_size = m_stackSize;
        makecontext(&m_ctx, (void (*)()) &Fiber::MainFunc, 0);
    }

    Fiber::~Fiber() {
        s_fiber_count--;
        if(m_stack){
            //有栈空间为子协程
            HH_ASSERT(m_state==INIT || m_state==TERM || m_state==EXCEPT)
            StackAllocator::Dealloc(m_stack,m_stackSize);
        }else{
            HH_ASSERT(m_state==EXEC);
            HH_ASSERT(m_cb);
            if(t_fiber==this)
                SetThis(nullptr);
        }

    }
}