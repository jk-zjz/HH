//
// Created by 35148 on 2024/7/15.
//
#include "tcp_server.h"
#include "log.h"
#include "config.h"
namespace hh{
    static hh::Logger::ptr g_logger = HH_LOG_NAME("system");
    static  hh::ConfigVar<uint64_t>::ptr g_tcp_server_recv_timeout =
            hh::Config::Lookup("tcp_server.RecvTimeout",(uint64_t)(60*1000*2),"tcp server recv timeout");
    TcpServer::TcpServer(IOManager *worker,
                         IOManager* acceptWorker):
                         m_worker(worker),
                         m_acceptWorker(acceptWorker),
                         m_recvTimeout(g_tcp_server_recv_timeout->getValue()),
                         m_isStop(true),
                         m_name("HH/1.0.0"){

    }
    bool TcpServer::bind(const Address::ptr &address){
        std::vector<Address::ptr>address_;
        std::vector<Address::ptr>fails;
        address_.emplace_back(address);
        return bind(address_,fails);
     }
    bool TcpServer::bind(const std::vector<Address::ptr> &address,
                         std::vector<Address::ptr> &fails){
        for(auto &addr:address){
            // 创建TCP连接socket
            Socket::ptr sock =hh::Socket::CreateTCP(addr);
            // 绑定
            if(!sock->bind(addr)){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<
                "bind address fail"<<errno<<" "<<strerror(errno)<<
                " "<<addr->toString();
                fails.emplace_back(addr);
                continue;
            }
            // 监听
            if(!sock->listen()){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<
                "listen address fail"<<errno<<" "<<strerror(errno)<<
                " "<<addr->toString();
                fails.emplace_back(addr);
                continue;
            }
            m_sockets.emplace_back(sock);
        }
        if(!fails.empty()){
            // 不为空
            m_sockets.clear();
            return false;
        }
        for(auto &sock:m_sockets){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<
            "TcpServer::bind success"<<*sock;
        }
        return true;
     }
    TcpServer::~TcpServer(){
        // 关闭socket，清空所有socket
        for(auto &sock:m_sockets){
            sock->close();
        }
        m_sockets.clear();
    }

    // 启动
    bool TcpServer::start(){
        if(!m_isStop){
            return true;
        }
        m_isStop = false;
        // 绑定回调，启动监听
        for(auto &sock:m_sockets){
            m_acceptWorker->schedule(std::bind(&TcpServer::startAccept,shared_from_this(),sock));
        }
        return true;
     }
    void TcpServer::stop(){
        m_isStop = true;
        auto self = shared_from_this();
        // 停止所有socket
        m_acceptWorker->schedule([this,self](){
            for(auto & sock:m_sockets){
                sock->cancelAll();
                sock->close();
            }
            m_sockets.clear();
        });
     }
     // 处理客户端连接
    void TcpServer::handleClient(Socket::ptr client){
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<
        "TcpServer::handleClient"<<*client;
    }
    // 监听
    void TcpServer::startAccept(Socket::ptr socke){
        while(!m_isStop){
            Socket::ptr client = socke->accept();
            if(client){
                client->setRecvTimeout(m_recvTimeout);
                m_worker->schedule(std::bind(&TcpServer::handleClient,shared_from_this(),client));
            }else{
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::ERROR)<<
                "accept errno="<<errno<<" "<<strerror(errno);
            }
        }
    }
}