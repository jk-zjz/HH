//
// Created by 35148 on 2024/6/7.
//
#include "iomanage.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include "macro.h"
namespace hh{
    IOManager::IOManager(int threads, bool use_caller, const std::string &name):
    Scheduler(threads, use_caller, name)
    {
        //创建fd句柄
        m_EpollFd= epoll_create(500);
        HH_ASSERT(m_EpollFd>0)
        //创建匿名管道
        int rt=pipe(m_TickleFd);
        HH_ASSERT(rt!=-1);
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        //设置读事件以及边沿触发
        ev.events=EPOLLIN | EPOLLET;
        ev.data.fd=m_TickleFd[0];
        //为当前fd设置非堵塞
        rt = fcntl(m_TickleFd[0], F_SETFL, O_NONBLOCK);
        HH_ASSERT(rt!=-1);
        //注册epoll事件
        rt=epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, m_TickleFd[0], &ev);
        HH_ASSERT(rt!=-1);
        fdcontextReset(32);
        //启动线程
        start();
    }

    IOManager::~IOManager() {
        stop();
        close(m_EpollFd);
        close(m_TickleFd[0]);
        close(m_TickleFd[1]);
        for(size_t i=0;i<m_fdContexts.size();i++){
            if(m_fdContexts[i]){
                delete m_fdContexts[i];
            }
        }
    }
    /**
     * 提取初始化fd上下文
     *
     * */
    void IOManager::fdcontextReset(int size) {
        m_fdContexts.resize(size);
        for(size_t i=0;i<m_fdContexts.size();i++){
            if(!m_fdContexts[i]){
                m_fdContexts[i]=new FdContext;
                m_fdContexts[i]=i;
            }
        }
    };

    int IOManager::addEvent(int fd, IOManager::Event event, std::function<void()> cb) {
        return 0;
    }
}
