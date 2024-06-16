//
// Created by 35148 on 2024/6/14.
//
#include "hook.h"
#include <dlfcn.h>
#include <asm-generic/ioctls.h>
#include "iomanage.h"
#include "macro.h"

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
    if (!hh::is_hook_enable()) {
        // 如果钩子功能未启用，则直接调用原始I/O操作函数
        return fun(fd, std::forward<Args>(args)...);
    }
    // 获取有没有套接字
    hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(fd);
    if (!ctx) {
        // 如果套接字不存在，则调用原始I/O操作函数
        return fun(fd, std::forward<Args>(args)...);
    }
    if (!ctx->isClose()) {
        // EBADF 错误表示套接字已关闭
        errno = EBADF;
        return -1;
    }
    if (!ctx->isSocket() || ctx->getUserNonblock()) {
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
    while (n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }
    // EAGAIN 表示资源暂时不可用，需要稍后再试
    if (n == -1 && errno == EAGAIN) {
        hh::IOManager *iom = hh::IOManager::GetThis();
        hh::Timer::ptr timer;
        // 创建一个定时器，用于在超时时间到期时取消I/O操作
        std::weak_ptr<timer_info> winfo(tinfo);
        if (to != (uint64_t) -1) {
            // 设置了超时时间
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();
                if (!t || t->cancelled) {
                    // 如果定时器已过期或已取消，则返回
                    return;
                }
                // 取消I/O操作 ETIMEDOUT表示超时
                t->cancelled = ETIMEDOUT;
                // 设置套接字为可读或可写
                iom->cancelEvent(fd, (hh::IOManager::Event) event);
            }, winfo);
        }
        int re = iom->addEvent(fd, (hh::IOManager::Event) event);
        if (HH_UNLIKELY(re)) {
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR)
                << "addEvent error" << hook_fun_name << "(" << fd << ")" << errno;
            if (timer) {
                // 如果有定时器，则取消定时器 强制触发掉任务
                timer->cancel();
            }
            return -1;
        } else {
            hh::Fiber::YieldToHold();
            if (timer) {
                timer->cancel();
            }
            if (tinfo->cancelled) {
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

    // 添加定时器 通过使用bind函数将定时器添加到IO管理器中，并指定了调度函数(schedule)和参数（iom,fiber,-1）
    iom->addTimer(seconds * 1000, std::bind((void (hh::Scheduler::*)
            (hh::Fiber::ptr, int thread)) &hh::IOManager::schedule, iom, fiber, -1));
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
    // 添加定时器 使用bind
    iom->addTimer(usec / 1000, std::bind((void (hh::Scheduler::*)
            (hh::Fiber::ptr, int thread)) &hh::IOManager::schedule, iom, fiber, -1));
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
int socket(int domain, int type, int protocol) {
    // 如果没有启用钩子功能，则直接调用原始的socket函数
    if (!hh::is_hook_enable()) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if (fd == -1) {
        HH_LOG_ERROR(g_logger, "socket error");
        return fd;
    }
    // 获取文件描述符管理器并设置文件描述符为非阻塞模式
    hh::FdMgr::GetInstance()->get(fd, true);
    return fd;
}
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    // 使用模板函数do_io来执行实际的I/O操作
    int fd = do_io(sockfd, accept_f, "accept", hh::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0) {
        // 获取文件描述符管理器并设置文件描述符为非阻塞模式
        hh::FdMgr::GetInstance()->get(fd, true);
        return fd;
    };
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", hh::IOManager::READ, SO_RCVTIMEO, buf, count);
}

// 可以分缓冲区写入到不同的地方
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", hh::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}


// 读 flags可以用来设置一些标志位最常用的为MSG_PEEK，读取数据但不删除，MSG_OOB，读取带外数据，MSG_DONTWAIT，非阻塞等
ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", hh::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

// 读 可以存储发送方的地址src_addr
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr,
                 socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", hh::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

// 读 msghdr 结构体的指针，该结构体描述了消息的缓冲区和辅助数据。
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", hh::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", hh::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

// 可以分缓冲区写入到不同的地方
ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", hh::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}


// 写 flags可以用来设置一些标志位最常用的为MSG_PEEK，读取数据但不删除，MSG_OOB，读取带外数据，MSG_DONTWAIT，非阻塞等
ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return do_io(sockfd, send_f, "send", hh::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

// 写 可以存储发送方的地址src_addr
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
               socklen_t addrlen) {
    return do_io(sockfd, sendto_f, "sendto", hh::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

// 写 msghdr 结构体的指针，该结构体描述了消息的缓冲区和辅助数据。
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return do_io(sockfd, sendmsg_f, "sendmsg", hh::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}
int close(int fd) {
    if (!hh::is_hook_enable()) {
        return close_f(fd);
    }
    // 获取文件描述符类，并且进行删除fd的所有事件
    hh::FdCtx::ptr ptr = hh::FdMgr::GetInstance()->get(fd);
    if (ptr) {
        auto iom = hh::IOManager::GetThis();
        if (iom) {
            iom->cancelAll(fd);
        }
    }
    return close_f(fd);
} ;

int fcntl(int fd, int cmd, ... /* arg */ ) {
    if (!hh::is_hook_enable()) {
        return fcntl_f(fd, cmd);
    }
    va_list va;
    va_start(va, cmd);
    switch (cmd) {
        // 处理F_SETFL命令：设置文件描述符标志
        case F_SETFL: {
            int arg = va_arg(va, int); // 获取va_list中的下一个参数，这里是int类型
            va_end(va); // 清空va_list，表明已处理完所有可变参数
            hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(fd); // 获取文件描述符对应的上下文
            if (!ctx || !ctx->isSocket() || ctx->isClose()) { // 检查上下文有效性及文件描述符状态
                return fcntl_f(fd, cmd, arg); // 上下文无效或文件描述符已关闭时，调用自定义fcntl处理函数
            }
            // 判断 用户态是否为非阻塞
            ctx->setUserNonblock(arg & O_NONBLOCK);
            if (ctx->getSysNonblock()) {
                // 设置系统非阻塞
                arg |= O_NONBLOCK;
            } else {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
            // 以下命令处理模式相似，主要区别在于参数类型和是否需要额外处理
        case F_GETFL: // 获取文件描述符标志
        {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(fd);
            if (!ctx || ctx->isClose() || !ctx->isSocket()) {
                return arg;
            }
            if (ctx->getUserNonblock()) {
                return arg | O_NONBLOCK;
            } else {
                return arg & ~O_NONBLOCK;
            }
        }
        case F_DUPFD: // 复制文件描述符
        case F_DUPFD_CLOEXEC: // 复制文件描述符并在execve后关闭
        case F_SETFD: // 设置文件描述符 close-on-exec 标志
        case F_SETOWN: // 设置文件描述符的所有者进程ID
        case F_SETSIG: // 设置异步I/O信号
        case F_SETLEASE: // 设置文件租约
        case F_NOTIFY: // 文件或目录变化通知
        case F_SETPIPE_SZ: // 设置管道缓冲区大小
        {
            int arg = va_arg(va, int); // 获取整型参数
            va_end(va); // 结束可变参数处理
            return fcntl_f(fd, cmd, arg); // 调用自定义fcntl处理函数，传入参数
        }
            break;
            // 下列命令无需额外参数，直接调用fcntl处理
        case F_GETFD: // 获取文件描述符的close-on-exec标志
        case F_GETOWN: // 获取文件描述符的所有者进程ID
        case F_GETSIG: // 获取异步I/O信号
        case F_GETLEASE: // 获取文件租约信息
        case F_GETPIPE_SZ: // 获取管道缓冲区大小
        {
            va_end(va); // 结束可变参数处理
            return fcntl_f(fd, cmd); // 调用自定义fcntl处理函数
        }
            break;
            // 处理文件锁相关的命令
        case F_SETLK: // 设置记录锁（非阻塞）
        case F_SETLKW: // 设置记录锁（阻塞直到成功）
        case F_GETLK: // 检查记录锁状态
        {
            struct flock *arg = va_arg(va, struct flock *); // 获取文件锁结构体指针
            va_end(va); // 结束可变参数处理
            return fcntl_f(fd, cmd, arg); // 调用自定义fcntl处理函数，传入文件锁结构体
        }
            break;
            // 处理进程所有权扩展相关命令
        case F_GETOWN_EX: // 获取扩展的所有权信息
        case F_SETOWN_EX: // 设置扩展的所有权信息
        {
            struct f_owner_ex *arg = va_arg(va, struct f_owner_ex *); // 获取所有权扩展结构体指针
            va_end(va); // 结束可变参数处理
            return fcntl_f(fd, cmd, arg); // 调用自定义fcntl处理函数，传入所有权扩展结构体
        }
            break;
            // 默认情况，处理未列出的其他命令
        default: {
            va_end(va); // 结束可变参数处理
            return fcntl(fd, cmd); // 直接调用系统提供的fcntl函数
        }
    }
} ;

/**
 * ioctl函数 设置非阻塞
 * @param fd
 * @param request
 * @param ...
 * @return
 */
int ioctl(int fd, unsigned long request, ...) {
    // ... 是一个void * 的int型，1 是设置非阻塞，0是阻塞状态
    va_list va;
    va_start(va, request);
    void *arg = va_arg(va, void *);
    va_end(va);
    if (request == FIONBIO) {
        bool user_nonblock = !!*(int *) arg;
        hh::FdCtx::ptr ptr = hh::FdMgr::GetInstance()->get(fd);
        if (!ptr || !ptr->isSocket() || ptr->isClose()) {
            return ioctl_f(fd, request, arg);
        }
        ptr->setUserNonblock(user_nonblock);
    }
    return ioctl_f(fd, request, arg);
} ;

// 获取文件句柄访问状态
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
} ;

/**
 * 设置文件句柄访问状态
 * @param sockfd
 * @param level
 * @param optname
 * @param optval
 * @param optlen
 * @return
 */
/*
sockfd：套接字文件描述符。
level：选项级别，使用 SOL_SOCKET 表示套接字级别。
optname：选项名称。
optval：指向选项值的指针。
optlen：选项值的长度。
*/
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!hh::is_hook_enable()) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if (level == SOL_SOCKET) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(sockfd);
            if (ctx) {
                const struct timeval *tv = (const struct timeval *) optval;
                ctx->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
} ;

int connect_with_timeout(int fd, const struct sockaddr *addr, socklen_t addrlen, int timeout_ms) {
    if(!hh::is_hook_enable()){
        return connect_f(fd, addr, addrlen);
    }
    hh::FdCtx::ptr ptr = hh::FdMgr::GetInstance()->get(fd);
    if (!ptr || ptr->isClose()) {
        // 已经关闭 错误
        errno = EBADF;
        return -1;
    }
    if(!ptr->isSocket()){
        // 不是一个socket
        return connect_f(fd, addr, addrlen);
    }
    // 用户态非阻塞
    if(ptr->getUserNonblock()){
        return connect_f(fd, addr, addrlen);
    }
    hh::IOManager *iom = hh::IOManager::GetThis();
    hh::Timer::ptr timer;
    std::shared_ptr<timer_info> info(new timer_info);
    std::weak_ptr<timer_info> winfo(info);
    // 如果拥有超时时间
    if(timeout_ms != (uint64_t)-1){
        timer = iom->addConditionTimer(timeout_ms, [winfo,fd,iom]() {
            auto t = winfo.lock();
            if(!t || t->cancelled){
                return;
            }
            t->cancelled = ETIMEDOUT;
            iom->cancelEvent(fd,hh::IOManager::WRITE);
        }, winfo);
    }
    int rt = connect_f(fd, addr, addrlen);
    // 成功返回0，失败-1
    if(rt == 0){
        // 成功直接hold
        hh::Fiber::YieldToHold();
        if(timer){
            timer->cancel();
        }
        if(info->cancelled){
            errno = info->cancelled;
            return -1;
        }
    }else{
        if(timer){
            timer->cancel();
        }
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<"connect error";
    }
    int error = 0;
    socklen_t len = sizeof(int);
    if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
        return -1;
    }
    if(error == 0){
        // 成功
        return 0;
    }else{
        // 失败
        errno = error;
        return -1;
    }
}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if(!hh::is_hook_enable()){
        //return connect_with_timeout(sockfd, addr, addrlen,hh::s);
    }
    return connect_f(sockfd, addr, addrlen);
}

}