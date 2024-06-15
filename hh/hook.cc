//
// Created by 35148 on 2024/6/14.
//
#include "hook.h"
#include <dlfcn.h>
#include "fiber.h"
#include "iomanage.h"
#include "macro.h"
#include <ctime>
hh::Logger::ptr g_logger = HH_LOG_NAME("system");
namespace hh {
    thread_local bool t_enable_hook_guard = false;
#define HOOK_FUN(XX) \
    XX(sleep)         \
    XX(usleep)       \
    XX(nanosleep)    \
    XX(socket)       \
    XX(accept)           \
    XX(connect)          \
    XX(read)             \
    XX(readv)            \
    XX(recv)             \
    XX(recvfrom)         \
    XX(recvmsg)          \
    XX(write)            \
    XX(writev)           \
    XX(send)             \
    XX(sendto)           \
    XX(sendmsg)          \
    XX(close)            \
    XX(fcntl)            \
    XX(ioctl)            \
    XX(getsockopt)       \
    XX(setsockopt)

    void hook_init() {
        static bool s_inited = false;
        if (s_inited) {
            return;
        }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
        HOOK_FUN(XX);
#undef XX
    }

    struct _FuncInit {
        _FuncInit() {
            hook_init();
        };
    };

    static _FuncInit s_init;

    bool is_hook_enable() {
        return t_enable_hook_guard;
    };

    void set_hook_enable(bool flag) {
        t_enable_hook_guard = flag;
    }

}
struct timer_info {
    int cancelled = 0;
};

// 通用异步I/O操作模板函数
template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun *fun,
                     const char *hook_fun_name, // I/O操作对应的钩子函数名称，用于调试或监控
                     uint32_t event,            // 事件类型，比如读、写或同时读写事件
                     int timeout_so,            // 套接字超时设置的选项，如SO_RCVTIMEO或SO_SNDTIMEO
                     Args &&... args) {         // 可变参数模板，转发给原始I/O操作的所有其他参数
    if(!hh::is_hook_enable()){
        // 如果钩子功能未启用，则直接调用原始I/O操作函数
        return fun(fd, std::forward<Args>(args)...);
    }
    // 获取有没有套接字
    hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(fd);
    if (!ctx) {
        // 如果套接字不存在，则调用原始I/O操作函数
        return fun(fd, std::forward<Args>(args)...);
    }
    if(!ctx->isClose()){
        // EBADF 错误表示套接字已关闭
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket() || ctx->getUserNonblock()){
        // 如果套接字不存在，则调用原始I/O操作函数
        return fun(fd, std::forward<Args>(args)...);
    }
    // 获取套接字超时时间 读或写
    uint64_t to = ctx->getTimeout(timeout_so);
    // 创建一个定时器，用于在超时时间到期时取消I/O操作
    std::shared_ptr<timer_info> tinfo(new timer_info);

hh:
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    // EINTR 表示系统调用被信号中断
    while (n == -1 && errno == EINTR){
        n = fun(fd, std::forward<Args>(args)...);
    }
    // EAGAIN 表示资源暂时不可用，需要稍后再试
    if (n == -1 && errno == EAGAIN) {
        hh::IOManager *iom = hh::IOManager::GetThis();
        hh::Timer::ptr timer;
        // 创建一个定时器，用于在超时时间到期时取消I/O操作
        std::weak_ptr<timer_info> winfo(tinfo);
        if(to != (uint64_t)-1){
            // 设置了超时时间
            timer = iom->addConditionTimer(to, [winfo, fd,iom,event]() {
                auto t = winfo.lock();
                if (!t || t->cancelled) {
                    // 如果定时器已过期或已取消，则返回
                    return;
                }
                // 取消I/O操作 ETIMEDOUT表示超时
                t->cancelled =ETIMEDOUT;
                // 设置套接字为可读或可写
                iom->addEvent(fd, (hh::IOManager::Event)event);
            },winfo);
        }
        int re = iom->addEvent(fd, (hh::IOManager::Event)event);
        if(HH_UNLIKELY(re)){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)
            <<"addEvent error"<<hook_fun_name<<"("<<fd<<")"<<errno;
            if(timer){
                // 如果有定时器，则取消定时器 强制触发掉任务
                timer->cancel();
            }
            return -1;
        }else{
            hh::Fiber::YieldToHold();
            if(timer){
                timer->cancel();
            }
            if(tinfo->cancelled){
                errno = tinfo->cancelled;
                return -1;
            }
            goto hh;
        }
    }
    return n;
}
extern "C" {
#define XX(name) name ## _fun name ## _f = nullptr;
HOOK_FUN(XX) ;
#undef XX

/**
 * 重载实现sleep函数，以支持在特定并发框架(hh)中进行非阻塞式延时。
 * 当钩子功能启用时，将使用携程调度和定时器来模拟延时，否则调用原始sleep函数。
 *
 * @param seconds 需要睡眠的秒数
 * @return 返回值0表示成功执行
 */
unsigned int sleep(unsigned int seconds) {
    // 如果没有启用钩子功能，则直接调用原始的sleep函数
    if (!hh::is_hook_enable()) {
        return sleep_f(seconds); // sleep_f指向原始sleep函数
    }

    // 获取当前激活的携程对象和当前的IO管理器实例
    hh::Fiber::ptr fiber = hh::Fiber::GetThis();
    hh::IOManager *iom = hh::IOManager::GetThis();

    // 向IO管理器注册一个定时器，等待seconds秒（单位转换为毫秒）
    // 定时到达后，通过回调函数重新调度当前携程执行
    iom->addTimer(seconds * 1000, [iom, fiber]() {
        iom->schedule(fiber); // 调度携程继续执行
    });

    // 主动放弃CPU，让出控制权给携程调度器
    hh::Fiber::YieldToHold();

    return 0;
}

/**
 * 类似地，重载usleep函数以支持微秒级别的非阻塞延时。
 *
 * @param usec 需要延时的微秒数
 * @return 返回值0表示成功执行
 */
int usleep(useconds_t usec) {
    if (!hh::is_hook_enable()) {
        return usleep_f(usec); // 直接调用原始的usleep函数
    }

    hh::Fiber::ptr fiber = hh::Fiber::GetThis();
    hh::IOManager *iom = hh::IOManager::GetThis();

    // 将微秒转换为毫秒后，添加定时器
    iom->addTimer(usec / 1000, [iom, fiber]() {
        iom->schedule(fiber); // 定时结束时恢复携程执行
    });

    // 携程让出
    hh::Fiber::YieldToHold();

    return 0;
}
/**
 * 重载nanosleep函数以支持纳秒级别的非阻塞延时。
 * @param req
 * @param rem
 * @return
 */
int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!hh::is_hook_enable()) {
        return nanosleep_f(req, rem);
    }
    // 将timespec结构体转换为毫秒
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    hh::Fiber::ptr fiber = hh::Fiber::GetThis();
    hh::IOManager *iom = hh::IOManager::GetThis();

    iom->addTimer(timeout_ms, [iom, fiber]() {
        iom->schedule(fiber);
    });
    hh::Fiber::YieldToHold();
    return 0;
}


}