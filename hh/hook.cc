//
// Created by 35148 on 2024/6/14.
//
#include "hook.h"
#include <dlfcn.h>
#include "fiber.h"
#include "iomanage.h"
#include <time.h>

namespace hh {
    thread_local bool t_enable_hook_guard = false;
#define HOOK_FUN(XX) \
    XX(sleep)         \
    XX(usleep)       \
    XX(nanosleep)    \
    XX(socket)



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