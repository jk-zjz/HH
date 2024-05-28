//
// Created by 35148 on 2024/5/23.
//
#include "fiber.h"
#include <atomic>
#include <utility>
#include "config.h"
#include "macro.h"
namespace hh {
    static Logger::ptr g_logger = HH_LOG_NAME("system");
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
        //创建主协程
        m_state = EXEC;
        SetThis(this);
        if (getcontext(&m_ctx)) {
            HH_ASSERT2(false, "Fiber::Fiber");
        }
        ++s_fiber_count;
    }

    Fiber::Fiber(std::function<void()> cb, size_t stackSize) :
            m_cb(std::move(cb)),
            m_id(++s_fiber_id){
        //创建子协程
        ++s_fiber_count;
        m_stackSize=stackSize ?stackSize:g_fiber_stack_size->getValue();
        m_stack = StackAllocator::Alloc(m_stackSize);
        if (getcontext(&m_ctx)) {
            HH_ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        //指定栈空间
        m_ctx.uc_stack.ss_sp = m_stack;
        //指定栈大小
        m_ctx.uc_stack.ss_size = m_stackSize;
        makecontext(&m_ctx, (void (*)()) &Fiber::MainFunc, 0);
    }

    Fiber::~Fiber() {
        --s_fiber_count;
        if(m_stack){
            //有栈空间为子协程
            HH_ASSERT(m_state==INIT || m_state==TERM || m_state==EXCEPT)
            StackAllocator::Dealloc(m_stack,m_stackSize);
        }else{
            //主协程
            HH_ASSERT(m_state==EXEC);
            HH_ASSERT(!m_cb);
            Fiber* cur = t_fiber;
            if(cur==this)
                SetThis(nullptr);
        }
    }


    void Fiber::SetThis(Fiber *f) {
        t_fiber = f;
    }

    void Fiber::reset(std::function<void()> cb) {
        HH_ASSERT(m_stack);
        HH_ASSERT(m_state == INIT || m_state == TERM|| m_state == EXCEPT);
        m_cb = std::move(cb);
        if(getcontext(&m_ctx)){
            HH_ASSERT2(false,"Fiber::reset");
        }
        //后续上下文置空，以免使用了已经执行完协程的后续上下文
        m_ctx.uc_link = nullptr;
        //指定栈空间
        m_ctx.uc_stack.ss_sp = m_stack;
        //指定栈大小
        m_ctx.uc_stack.ss_size = m_stackSize;
        makecontext(&m_ctx, (void (*)()) &Fiber::MainFunc, 0);
        m_state = INIT;
    }

    //切换到当前协程执行
    void Fiber::swapIn() {
        SetThis(this);
        HH_ASSERT(m_state != EXEC);
        m_state = EXEC;
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            HH_ASSERT2(false, "Fiber::swapIn");
        }
    }
    //唤醒主协程中的上下文，吧当前协程切换到后台
    void Fiber::swapOut() {
        SetThis(t_threadFiber.get());
        if (swapcontext(&m_ctx,&t_threadFiber->m_ctx)) {
            HH_ASSERT2(false, "Fiber::swapOut");
        }
    }

    Fiber::ptr Fiber::GetThis() {
        if(t_fiber){
            return t_fiber->shared_from_this();
        }
        //没有初始化主协程
        Fiber::ptr main_fiber(new Fiber);
        HH_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
        return t_fiber->shared_from_this();
    }

    uint64_t Fiber::GetTotalFibers() {
        return s_fiber_count;
    }

    void Fiber::YieldToHold() {
        Fiber::ptr u_fiber= GetThis();
        u_fiber->m_state=HOLD;
        u_fiber->swapOut();
    }

    void Fiber::YieldToReady() {
        Fiber::ptr u_fiber= GetThis();
        u_fiber->m_state=READY;
        u_fiber->swapOut();
    }

    void Fiber::MainFunc() {
        Fiber::ptr m_fiber = GetThis();
        HH_ASSERT(m_fiber);
        try {
            m_fiber->m_cb();
            m_fiber->m_cb= nullptr;
            m_fiber->m_state = TERM;
        }catch(std::exception &e) {
            m_fiber->m_state = EXCEPT;
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<"Fiber Except: "
                                                            <<e.what();
        }catch (...) {
            //未知异常
            m_fiber->m_state = EXCEPT;
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<"Fiber Except: "
                                                            <<"unknown exception";
        }
        auto pFiber = m_fiber.get();
        m_fiber.reset();
        pFiber->swapOut();
    }

    uint32_t Fiber::getFiber_id() {
        if(t_fiber){
            return t_fiber->m_id;
        }
        return 0;
    }
}