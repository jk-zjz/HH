//
// Created by 35148 on 2024/6/15.
//

#ifndef HH_FD_MANAGER_H
#define HH_FD_MANAGER_H

#include <cstdint>
#include <vector>
#include <memory>
#include "thread.h"
#include "hook.h"
#include "singleton.h"


namespace hh {
    /**
     * FdCtx类用于管理套接字相关的上下文信息，包括初始化、关闭、获取超时时间等操作。
     */
    class FdCtx : public std::enable_shared_from_this<FdCtx>{
    public:
        typedef std::shared_ptr<FdCtx> ptr;

        FdCtx(int fd);
        ~FdCtx();

        // 初始化套接字及其相关配置
        bool init();

        // 检查套接字是否已完成初始化
        bool isInit() const{return m_isInited;};

        // 查询当前对象是否代表一个有效的套接字
        bool isSocket() const{return m_isSocket;};

        // 检查套接字是否已关闭
        bool isClose() const {return m_isClosed;};

        // 关闭套接字，释放相关资源
        bool close();

        // 获取系统级非阻塞标志的状态
        bool getSysNonblock() const {return m_sysNonblock;};

        // 设置系统级的非阻塞模式
        void setSysNonblock(bool value){m_sysNonblock = value;};

        // 获取用户自定义的非阻塞标志状态
        bool getUserNonblock() const {return m_userNonblock;};

        // 设置用户自定义的非阻塞模式
        void setUserNonblock(bool value){m_userNonblock = value;};

        // 根据类型（接收或发送）获取超时时间
        uint64_t getTimeout(int type) const{return type == SO_RCVTIMEO ? m_timeout[0] : m_timeout[1];};

        // 设置超时时间，type指定是接收还是发送超时，v为新的超时时间值
        void setTimeout(int type, uint64_t v);

    private:
        bool m_isInited: 1;      // 标记套接字是否已完成初始化
        bool m_isClosed: 1;     // 表示套接字是否已被关闭
        bool m_sysNonblock: 1;  // 指示是否设置了系统级的非阻塞模式
        bool m_userNonblock: 1; // 用户自定义的非阻塞标志，用于额外控制逻辑
        bool m_isSocket: 1;     // 标识该对象是否为一个有效的套接字
        int m_sockfd;            // 套接字的文件描述符，用于操作套接字
        // 超时设置，单位通常为毫秒或微秒，取决于具体应用上下文
            // 接收数据的超时时间 0
            // 发送数据的超时时间 1
        uint64_t m_timeout[2];
    };
    class FdManager {
    public:
        typedef RWMutex RWMutexType;\
        FdManager();
        // 根据文件描述符获取对应的FdCtx对象
        FdCtx::ptr get(int fd, bool auto_create = false);
        // 关闭文件描述符对应的套接字
        void del(int fd);
    private:
        RWMutexType m_mutex;
        std::vector<FdCtx::ptr> m_datas;
    };
    typedef Singleton<FdManager> FdMgr;
}
#endif //HH_FD_MANAGER_H
