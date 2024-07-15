//
// Created by 35148 on 2024/7/15.
//

#ifndef HH_TCP_SERVER_H
#define HH_TCP_SERVER_H
#include <memory>
#include <vector>
#include "iomanage.h"
#include "socket.h"
namespace hh{
    class TcpServer: public std::enable_shared_from_this<TcpServer>,
                     Noncopyable{
    public:
        typedef std::shared_ptr<TcpServer>  ptr;
        TcpServer(IOManager *worker = IOManager::GetThis(),
                  IOManager* acceptWorker = IOManager::GetThis());
        virtual bool bind(const Address::ptr &address); // 绑定地址
        virtual bool bind(const std::vector<Address::ptr> &address
                        ,std::vector<Address::ptr> &fails);
        virtual bool start();   // 启动
        virtual void stop();    // 停止

        virtual ~TcpServer();

        std::string getName()const {return m_name;}
        virtual void setName(const std::string &name) {m_name = name;}

        uint64_t getRecvTimeout()const {return m_recvTimeout;}
        void setRecvTimeout(uint64_t v) {m_recvTimeout = v;}

        bool isStop()const {return m_isStop;}
    protected:
        // 处理客户端连接
        virtual void handleClient(Socket::ptr client);
        // 监听socket
        virtual void startAccept(Socket::ptr socke);
    private:
        std::vector<Socket::ptr> m_sockets;     // 监听的socket
        IOManager *m_worker;                    // 工作线程
        IOManager *m_acceptWorker;              // 监听线程
        uint64_t m_recvTimeout;                 // 接收超时时间
        bool m_isStop;                          // 是否停止
        std::string m_name;                     // 服务器名称
    };
}
#endif //HH_TCP_SERVER_H
