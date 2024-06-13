//
// Created by 35148 on 2024/6/6.
//

#ifndef HH_IOMANAGE_H
#define HH_IOMANAGE_H

#include "scheduler.h"
#include "timer.h"
/**
 * @class IOManager IO协程调度模块
 *
 * */
namespace hh {
    class IOManager : public Scheduler,public TimerManager{
    public:
        typedef std::shared_ptr<IOManager> ptr;
        typedef RWMutex RWMutexType;
        //事件
        enum Event {
            NONE = 0x0, //无事件
            READ = 0x1, //读事件
            WRITE = 0x4 //写事件
        };
    private:
        //FD上下文   FD指的是epoll文件描述符
        struct FdContext {
            typedef Mutex MutexType;
            //事件上下文
            struct EventContext {
                Scheduler *scheduler = nullptr; //协程调度器
                Fiber::ptr fiber;               //协程
                std::function<void()> cb;       //回调函数
            };
            EventContext & getEventContext(Event event);
            void resetEventContext(EventContext& eventContext);
            void triggerEvent(Event event);
            EventContext read;                  //读事件
            EventContext write;                 //写事件
            MutexType mutex;                    //互斥锁
            int fd = 0;                         //FD句柄
            Event events = NONE;                //事件状态
        };
        void fdcontextReset(int size);
    public:
        IOManager(int threads = 1, bool use_caller = true, const std::string &name = "");

        ~IOManager() override;

        //添加事件
        int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

        //删除事件
        bool delEvent(int fd, Event event);

        //取消事件--->不是删除而是强制触发事件
        bool cancelEvent(int fd, Event event);

        //取消所有事件
        bool cancelAll(int fd);

        static IOManager *GetThis();
    protected:
        void tickle() override;
        void idle() override;
        bool stopping() override;
        bool stopping(uint64_t &timeout);
        void onTimerInsertedAtFront() override;
    private:
        int m_EpollFd = 0;                                  //epoll文件描述符
        int m_TickleFd[2];                                  //用于唤醒epoll的文件描述符
        RWMutexType m_RWMutex;
        std::vector<FdContext *> m_fdContexts;              //FD上下文
        std::atomic<size_t> m_pendingEventCount = {0};    //等待处理的事件数量
    };
}


#endif //HH_IOMANAGE_H
