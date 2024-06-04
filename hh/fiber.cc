//
// Created by 35148 on 2024/5/23.
//
#include "fiber.h"
#include <atomic>
#include <utility>
#include "config.h"
#include "macro.h"
#include "scheduler.h"

namespace hh {
    static Logger::ptr g_logger = HH_LOG_NAME("system");

    //全局原子计数器
    static std::atomic<uint32_t> s_fiber_id{0};
    static std::atomic<uint32_t> s_fiber_count{0};

    //当前线程
    static thread_local Fiber *t_fiber = nullptr;
    //主 协程
    static thread_local Fiber::ptr t_threadFiber = nullptr;

    //注册fiber栈大小配置
    static hh::ConfigVar<uint32_t>::ptr g_fiber_stack_size = hh::Config::Lookup<uint32_t>("fiber.stack_size",
                                                                                          1024 * 1024, "stack size");

    //简单的内存分配器
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
        HH_LOG_FAT_DEBUG(g_logger, "Fiber::Fiber,id=%d", m_id);
    }

    /**
     * @brief Fiber类的构造函数
     *
     * 初始化一个Fiber对象，包括设置回调函数、协程ID、堆栈大小和堆栈内存分配。
     * 通过getcontext和makecontext函数配置协程的上下文环境，以便后续切换到该协程执行。
     *
     * @param cb 协程执行的回调函数
     * @param stackSize 协程的堆栈大小，如果为0，则使用默认堆栈大小
     * @param useCaller 是否使用调用者上下文作为协程的起始点
     */
    Fiber::Fiber(std::function<void()> cb, size_t stackSize, bool useCaller) :
            m_cb(std::move(cb)),
            m_id(++s_fiber_id) {
        ++s_fiber_count; // 增加协程计数器，用于统计当前系统中的协程数量
        // 设置堆栈大小，如果传入的stackSize为0，则使用默认堆栈大小
        m_stackSize = stackSize ? stackSize : g_fiber_stack_size->getValue();
        // 为协程分配堆栈内存
        m_stack = StackAllocator::Alloc(m_stackSize);
        // 初始化协程的上下文结构体
        if (getcontext(&m_ctx)) {
            HH_ASSERT2(false, "getcontext"); // 如果getcontext失败，断言失败
        }
        // 配置上下文结构体的链接和堆栈信息
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stackSize;
        // 根据useCaller参数决定使用哪种方式初始化协程上下文
        if (!useCaller) {
            // 如果不使用调用者上下文，直接调用Fiber::MainFunc作为入口
            makecontext(&m_ctx, (void (*)()) &Fiber::MainFunc, 0);
        } else {
            // 如果使用调用者上下文，调用Fiber::CallerMainFunc作为入口
            makecontext(&m_ctx, (void (*)()) &Fiber::CallerMainFunc, 0);
        }
    }


    Fiber::~Fiber() {
        --s_fiber_count;
        if (m_stack) {
            //有栈空间为子协程
            HH_ASSERT(m_state == INIT
                      || m_state == TERM
                      || m_state == EXCEPT)
            StackAllocator::Dealloc(m_stack, m_stackSize);
        } else {
            //主协程
            HH_ASSERT(m_state == EXEC);
            HH_ASSERT(!m_cb);
            Fiber *cur = t_fiber;
            if (cur == this)
                SetThis(nullptr);
        }
        HH_LOG_FAT_DEBUG(g_logger, "~Fiber,%d", m_id);
    }


    void Fiber::SetThis(Fiber *f) {
        t_fiber = f;
    }

    /**
     * 重新设置协程
     * 不为初始化状态 || 终止状态 || 异常状态 可重置
     * */
    void Fiber::reset(std::function<void()> cb) {
        HH_ASSERT(m_stack);
        HH_ASSERT(m_state == INIT
                  || m_state == TERM
                  || m_state == EXCEPT);
        m_cb = std::move(cb);
        if (getcontext(&m_ctx)) {
            HH_ASSERT2(false, "Fiber::reset");
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

    /**
     * @brief 切换到当前Fiber执行。
     *
     * 本函数将当前的执行上下文切换到当前Fiber对象所代表的线程中。在切换之前，会设置当前的Fiber对象，
     * 并确保Fiber的状态为执行中（EXEC）。如果切换上下文失败，则会触发断言。
     * 为调度器切换的
     */
    void Fiber::call() {
        SetThis(this); // 设置当前Fiber对象
        m_state = EXEC; // 将Fiber状态设置为执行中
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) { // 执行上下文切换
            HH_ASSERT2(false, "Fiber::call"); // 如果切换失败，触发断言
        }
    }

    /**
     * @brief 从当前Fiber返回到调度器。
     *
     * 本函数用于将执行上下文从当前Fiber切换回调度器所代表的线程。在切换之前，会恢复调度器的Fiber对象。
     * 如果切换上下文失败，则会触发断言。
     * 为调度器切换的
     */
    void Fiber::back() {
        SetThis(t_threadFiber.get()); // 恢复调度器的Fiber对象
        if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) { // 执行上下文切换
            HH_ASSERT2(false, "Fiber::back"); // 如果切换失败，触发断言
        }
    }

    /**
     * @brief 将当前Fiber切换为执行状态。
     *
     * 本函数用于将当前的Fiber对象切换到执行状态，从调度器的角度来看，相当于将当前的执行上下文从调度器
     * 切换到当前Fiber。在切换之前，会设置当前的Fiber对象，并确保Fiber的状态不是执行中。如果切换上下文
     * 失败，则会触发断言。
     * 为主协程切换的
     */
    void Fiber::swapIn() {
        SetThis(this); // 设置当前Fiber对象
        HH_ASSERT(m_state != EXEC); // 确保Fiber状态不是执行中
        m_state = EXEC; // 将Fiber状态设置为执行中
        if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) { // 执行上下文切换
            HH_ASSERT2(false, "Fiber::swapIn"); // 如果切换失败，触发断言
        }
    }

    /**
     * @brief 从当前Fiber切换回调度器。
     *
     * 本函数用于将当前的执行上下文从当前Fiber切换回调度器所代表的线程。在切换之前，会设置调度器的Fiber
     * 对象。如果切换上下文失败，则会触发断言。
     * 为主协程切换的
     */
    void Fiber::swapOut() {
        SetThis(Scheduler::GetMainFiber()); // 设置调度器的Fiber对象
        if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) { // 执行上下文切换
            HH_ASSERT2(false, "Fiber::swapOut"); // 如果切换失败，触发断言
        }
    }

    /**
     * @brief 获取当前Fiber对象 && 没有主协程会创建
     *
     * */
    Fiber::ptr Fiber::GetThis() {
        if (t_fiber) {
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

    /**
     * @brief 将当前Fiber切换到挂起状态。
     *
     * 本函数首先获取当前执行的Fiber对象，然后检查其状态是否为执行中（EXEC）。如果满足条件，将Fiber状态
     * 保持不变并执行上下文切换到调度器。
     */
    void Fiber::YieldToHold() {
        Fiber::ptr u_fiber = GetThis(); // 获取当前Fiber对象
        HH_ASSERT(u_fiber->m_state == EXEC); // 检查Fiber状态
        u_fiber->swapOut(); // 切换到调度器
    }

    /**
     * @brief 将当前Fiber切换到就绪状态。
     *
     * 本函数同样首先获取当前执行的Fiber对象，检查其状态是否为执行中（EXEC）。如果满足条件，将Fiber状态更改为就绪（READY）
     * 并执行上下文切换到调度器。
     */
    void Fiber::YieldToReady() {
        Fiber::ptr u_fiber = GetThis(); // 获取当前Fiber对象
        HH_ASSERT(u_fiber->m_state == EXEC); // 检查Fiber状态
        u_fiber->m_state = READY; // 设置Fiber状态为就绪
        u_fiber->swapOut(); // 切换到调度器
    }

    /**
     * @brief 主Fiber执行函数。
     *
     * 本函数用于执行Fiber的回调函数（m_cb），并在执行完成后或捕获到异常时，将Fiber状态更新为终止（TERM）或异常（EXCEPT）。
     * 最后，释放Fiber对象并将其上下文切换回调度器。
     * 为单携程切换的
     */
    void Fiber::MainFunc() {
        Fiber::ptr m_fiber = GetThis(); // 获取当前Fiber对象
        HH_ASSERT(m_fiber); // 检查Fiber对象是否有效
        try {
            m_fiber->m_cb(); // 执行Fiber回调
            m_fiber->m_cb = nullptr; // 清空回调
            m_fiber->m_state = TERM; // 设置状态为终止
        } catch (std::exception &e) {
            m_fiber->m_state = EXCEPT; // 设置状态为异常
            // 记录异常日志
        } catch (...) {
            m_fiber->m_state = EXCEPT; // 设置状态为异常
            // 记录未知异常日志
        }
        auto pFiber = m_fiber.get(); // 获取Fiber指针
        m_fiber.reset(); // 释放Fiber对象
        pFiber->swapOut(); // 切换到调度器
        HH_ASSERT2(false, "never reach fiber::mainfunc"); // 断言不应到达此位置
    }

    /**
     * @brief 调用者Fiber执行函数。
     *
     * 类似于MainFunc，本函数用于执行Fiber的回调函数，并在完成或捕获到异常时更新Fiber状态。
     * 最后，释放Fiber对象并将执行上下文切换回Fiber的原始调用者。
     * 为携程调度器切换的
     */
    void Fiber::CallerMainFunc() {
        Fiber::ptr fiber = GetThis(); // 获取当前Fiber对象
        HH_ASSERT(fiber); // 检查Fiber对象
        try {
            fiber->m_cb(); // 执行Fiber回调
            fiber->m_cb = nullptr; // 清空回调
            fiber->m_state = TERM; // 设置状态为终止
        } catch (std::exception &e) {
            fiber->m_state = EXCEPT; // 设置状态为异常
            // 记录异常日志
        } catch (...) {
            fiber->m_state = EXCEPT; // 设置状态为异常
            // 记录未知异常日志
        }
        auto pFiber = fiber.get(); // 获取Fiber指针
        fiber.reset(); // 释放Fiber对象
        pFiber->back(); // 切换回Fiber的原始调用者
        HH_ASSERT2(false, "never reach fiber::callermainfunc"); // 断言不应到达此位置
    }

    /**
     * @brief 获取当前Fiber的ID。
     *
     * 如果存在当前Fiber对象，返回其ID；否则返回0。
     *
     * @return uint32_t 当前Fiber的ID，或0表示无Fiber。
     */
    uint32_t Fiber::getFiber_id() {
        if (t_fiber) {
            return t_fiber->m_id; // 返回Fiber ID
        }
        return 0; // 无Fiber时返回0
    }

}