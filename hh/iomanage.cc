//
// Created by 35148 on 2024/6/7.
//
#include "iomanage.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include "macro.h"

namespace hh {
    static hh::Logger::ptr g_logger = HH_LOG_NAME("system");

    /**
     * 构造函数
     * 初始化携程调度器
     * 创建匿名管道 && 设置非堵塞 && 边缘触发
     * 创建epoll句柄 && 注册epoll事件
     * 启动线程
     * */
    IOManager::IOManager(int threads, bool use_caller, const std::string &name) :
            Scheduler(threads, use_caller, name) {
        //创建fd句柄
        m_EpollFd = epoll_create(500);
        HH_ASSERT(m_EpollFd > 0)
        //创建匿名管道
        int rt = pipe(m_TickleFd);
        HH_ASSERT(rt != -1);
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        //设置读事件以及边沿触发
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = m_TickleFd[0];
        //为当前fd设置非堵塞
        rt = fcntl(m_TickleFd[0], F_SETFL, O_NONBLOCK);
        HH_ASSERT(rt != -1);
        //注册epoll事件
        rt = epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, m_TickleFd[0], &ev);
        HH_ASSERT(rt != -1);
        fdcontextReset(32);
        //启动线程
        start();
    }

    /**
     * 析构关闭携程调度器
     * 关闭epoll句柄 && 关闭管道句柄 && 释放fd上下文
     * */
    IOManager::~IOManager() {
        stop();
        close(m_EpollFd);
        close(m_TickleFd[0]);
        close(m_TickleFd[1]);
        for (size_t i = 0; i < m_fdContexts.size(); i++) {
            if (m_fdContexts[i]) {
                delete m_fdContexts[i];
            }
        }
    }

    /**
     * 动态扩容m_fdContexts数组
     *
     * */
    void IOManager::fdcontextReset(int size) {
        //从最后开始扩缩容
        size_t l = m_fdContexts.size();
        m_fdContexts.resize(size);
        for (size_t i = l; i < m_fdContexts.size(); i++) {
            if (!m_fdContexts[i]) {
                m_fdContexts[i] = new FdContext;
                m_fdContexts[i]->fd = i;
            }
        }
    };

    /**
     * 条件指定监听时间给fd，并且注册epoll事件
     * */
    int IOManager::addEvent(int fd, IOManager::Event event, std::function<void()> cb) {
        FdContext *FdCtx = nullptr;
        //读锁判断
        RWMutexType::ReadLock lock(m_RWMutex);
        if ((int)m_fdContexts.size() > fd) {
            FdCtx = m_fdContexts[fd];
            lock.unlock();
        } else {
            lock.unlock();
            //写锁扩容
            RWMutexType::WriteLock lock2(m_RWMutex);
            while (fd < (int)m_fdContexts.size()) {
                fdcontextReset(m_fdContexts.size() * 1.5);
            }
            FdCtx = m_fdContexts[fd];
        }
        HH_ASSERT(FdCtx);
        //互斥锁操作
        FdContext::MutexType lock3(FdCtx->mutex);
        /**
         * 如果我们要添加的事件已经存在，那么就不添加了
         * 如果两个事件系统 & 就是 1
         * */
        if (FdCtx->events & event) {
            HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR) << "addEvent assert fd =" << fd
                                                          << " event =" << event << " fdCtx.events =" << FdCtx->events;
            HH_ASSERT(!(FdCtx->events & event));
        }
        //如果无事件 0 就条件事件EPOLL_CTL_ADD 否则 就修改事件EPOLL_CTL_MOD
        int op = FdCtx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLET | FdCtx->events | event;
        ev.data.ptr = FdCtx;
        int rt = epoll_ctl(m_EpollFd, op, fd, &ev);
        if (rt) {
            HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR)
                << "epoll_ctl assert fd =" << fd
                << " event =" << event << " fdCtx.events =" << FdCtx->events
                << " rt =" << rt << " errno =" << errno << " errstr =" << strerror(errno);
            return -1;
        }
        m_pendingEventCount++;
        FdCtx->events = (Event) (FdCtx->events | event);
        //回调的上下文 读 | 写
        FdContext::EventContext event_ctx = FdCtx->getEventContext(event);
        HH_ASSERT(!event_ctx.scheduler
                  && !event_ctx.cb
                  && !event_ctx.fiber);
        event_ctx.scheduler = Scheduler::GetThis();
        //指定回调函数
        if (cb) {
            event_ctx.cb.swap(cb);
        } else {
            //为指定回调NULL 添加主协程并且为执行态
            event_ctx.fiber = Fiber::GetThis();
            HH_ASSERT(event_ctx.fiber->getState() == Fiber::State::EXEC);
        }
        return 0;
    }

    /**
     * 删除事件
     * 创建新epoll_event 根据事件类型删除或者修改
     *      int op = mew_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
     * 并且通过resetEventContext做清理工作
     * */
    bool IOManager::delEvent(int fd, IOManager::Event event) {
        FdContext *FdCtx = nullptr;
        {
            RWMutexType::ReadLock lock(m_RWMutex);
            if (fd >= (int)m_fdContexts.size()) {
                return false;
            }
            FdCtx = m_fdContexts[fd];
        }
        FdContext::MutexType lock(FdCtx->mutex);
        //取出的句柄状态是否和事件一致
        if (!(FdCtx->events & event)) {
            //不一致
            return false;
        }
        //删除事件
        Event mew_event = (Event) (FdCtx->events & ~event);
        int op = mew_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLET | mew_event;
        ev.data.ptr = FdCtx;
        int rt = epoll_ctl(m_EpollFd, op, fd, &ev);
        if (rt) {
            HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR)
                << "epoll_ctl assert fd =" << fd
                << " event =" << mew_event << " fdCtx.events =" << FdCtx->events
                << " rt =" << rt << " errno =" << errno << " errstr =" << strerror(errno);
            return false;
        }
        --m_pendingEventCount;
        FdCtx->events = mew_event;
        FdContext::EventContext event_ctx = FdCtx->getEventContext(event);
        FdCtx->resetEventContext(event_ctx);
        return true;
    }

    /**
     * 取消事件 ！= 删除事件 因为取消需要强制触发
     * */
    bool IOManager::cancelEvent(int fd, IOManager::Event event) {
        FdContext *FdCtx = nullptr;
        {
            RWMutexType::ReadLock lock(m_RWMutex);
            if (fd >= (int)m_fdContexts.size()) {
                return false;
            }
            FdCtx = m_fdContexts[fd];
        }
        FdContext::MutexType lock(FdCtx->mutex);
        if (!(FdCtx->events & event)) {
            //不一致
            return false;
        }
        Event mew_event = (Event) (FdCtx->events & ~event);
        int op = mew_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLET | mew_event;
        ev.data.ptr = FdCtx;
        int rt = epoll_ctl(m_EpollFd, op, fd, &ev);
        if (rt) {
            HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR)
                << "epoll_ctl assert fd =" << fd
                << " event =" << mew_event << " fdCtx.events =" << FdCtx->events
                << " rt =" << rt << " errno =" << errno << " errstr =" << strerror(errno);
        }
        //触发事件
        FdCtx->triggerEvent(event);
        --m_pendingEventCount;
        return true;
    }

    /**
     * 取消所有事件
     * 取消fd并且触发所有的事件
     * */
    bool IOManager::cancelAll(int fd) {
        FdContext *FdCtx = nullptr;
        {
            RWMutexType::ReadLock lock(m_RWMutex);
            if (fd >= (int)m_fdContexts.size()) {
                return false;
            }
            FdCtx = m_fdContexts[fd];
        }
        if (!(FdCtx->events)) {
            //为空时间就返回
            return false;
        }
        FdContext::MutexType lock(FdCtx->mutex);

        int rt = epoll_ctl(m_EpollFd, EPOLL_CTL_DEL, fd, nullptr);
        if (rt) {
            HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR)
                << "epoll_ctl assert fd =" << fd
                << " event =" << FdCtx->events << " fdCtx.events =" << FdCtx->events
                << " rt =" << rt << " errno =" << errno << " errstr =" << strerror(errno);
        }
        //触发掉都是无状态
        if (FdCtx->events & Event::READ) {
            FdCtx->triggerEvent(Event::READ);
            --m_pendingEventCount;
        }
        if (FdCtx->events & Event::WRITE) {
            FdCtx->triggerEvent(Event::WRITE);
            --m_pendingEventCount;
        }
        HH_ASSERT(FdCtx->events == Event::NONE);
        return true;
    }

    /**
     * 安全转换父转子
     * */
    IOManager *IOManager::GetThis() {
        return dynamic_cast<IOManager *>(Scheduler::GetThis());
    }

    /**
     * 获取事件上下文的回调
     * */
    IOManager::FdContext::EventContext &IOManager::FdContext::getEventContext(IOManager::Event event) {
        switch (event) {
            case IOManager::Event::READ:
                return read;
            case IOManager::Event::WRITE:
                return write;
            default:
                HH_ASSERT(false);
        }
    }

    /**
     * 事件清理
     * */
    void IOManager::FdContext::resetEventContext(IOManager::FdContext::EventContext eventContext) {
        eventContext.fiber.reset();
        eventContext.scheduler = nullptr;
        eventContext.cb = nullptr;
    }

    /**
     * 触发事件---通过协程调度器多线程执行
     * */
    void IOManager::FdContext::triggerEvent(IOManager::Event event) {
        HH_ASSERT(events & event);
        events = (Event) (events & ~event);
        EventContext event_ctx = getEventContext(event);
        if (event_ctx.cb) {
            event_ctx.scheduler->schedule(&event_ctx.cb);
        } else {
            event_ctx.scheduler->schedule(&event_ctx.fiber);
        }
        event_ctx.scheduler = nullptr;
    }

    //核心   完成 epoll调度
    void IOManager::tickle() {
        if (!hasIdleThreads()) {
            return;
        }
        int rt = write(m_TickleFd[1], "T", 1);
        HH_ASSERT(rt == 1);
    }

    bool IOManager::stopping() {
        //1.判断是否在停止状态 && 2.判断是否有事件
        return Scheduler::stopping() && m_pendingEventCount == 0;
    }

    void IOManager::idle() {
        //创建事件触发数组
        static int m_epollEventSize = 255;
        epoll_event *events = new epoll_event[m_epollEventSize];
        std::shared_ptr<epoll_event> event_ptr(events, [](epoll_event *ptr) {
            delete[] ptr;
        });
        while(true){
            if(stopping()){
                HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::INFO) << "idle stopping";
                break;
            }
            int rt = 0;
            do {
                //等待时间
                static const int MAX_TIMEOUT = 3000;
                rt = epoll_wait(m_EpollFd, events, m_epollEventSize,MAX_TIMEOUT);
                //没有时间 && 是被中断了需要重新获取
                if(rt == -1 && errno == EINTR){
                    continue;
                }else{
                    break;
                }
            } while (true);
            //触发了
            for(int i = 0; i < rt; ++i){
                //取出一个触发事件
                epoll_event &event = events[i];
                if(event.data.fd == m_TickleFd[0]){
                    char dummy;
                    //读出数据，清空管到  因为使用到边沿不然就触发不了了
                    while(read(m_TickleFd[0], &dummy, 1) == 1){
                        continue;
                    }
                }
                //取出上下文
                FdContext *FdCtx = (FdContext *)event.data.ptr;
                FdContext::MutexType lock(FdCtx->mutex);
                //判断event.events中是否有错误事件
                if(event.events & (EPOLLERR | EPOLLHUP)){
                    //没有就为event.events设置读写
                    event.events |= (EPOLLIN | EPOLLOUT) & FdCtx->events;
                }
                int real_events = 0;
                if(event.events & EPOLLIN){
                    //触发读事件
                    real_events |= Event::READ;
                }
                if(event.events & EPOLLOUT){
                    //触发写事件
                    real_events |= Event::WRITE;
                }
                if((FdCtx->events & real_events) == NONE){
                    continue;
                }
                // -去掉触发事件 剩余的事件
                int left_events = (FdCtx->events & ~real_events);
                int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
                event.events = EPOLLET | left_events;
                int rt2 = epoll_ctl(m_EpollFd, op, FdCtx->fd, &event);
                if(rt2){
                    HH_LOG_LEVEL_CHAIN(g_logger, LogLevel::ERROR)
                        << "epoll_ctl assert fd =" << FdCtx->fd
                        << " event =" << FdCtx->events << " fdCtx.events =" << FdCtx->events
                        << " rt =" << rt2 << " errno =" << errno << " errstr =" << strerror(errno);
                }
                //触发事件
                if(real_events & Event::READ){
                    FdCtx->triggerEvent(Event::READ);
                    --m_pendingEventCount;
                }
                if(real_events & Event::WRITE){
                    FdCtx->triggerEvent(Event::WRITE);
                    --m_pendingEventCount;
                }
            }
            Fiber::ptr cur = Fiber::GetThis();
//            Fiber::YieldToHold();    我觉得-----------------------
            auto it = cur.get();
            cur.reset();
            it->swapOut();
        }
    }
}
