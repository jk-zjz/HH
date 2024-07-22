//
// Created by 35148 on 2024/7/19.
//

#ifndef HH_HTTP_SERVER_H
#define HH_HTTP_SERVER_H
#include "tcp_server.h"
#include "servlet.h"
namespace hh{
    namespace http{
        class HttpServer : public TcpServer{
        public:
            typedef std::shared_ptr<HttpServer> ptr;
            HttpServer(bool keepalive = false,
                       hh::IOManager *worker = hh::IOManager::GetThis(),
                       hh::IOManager *accept_worker = hh::IOManager::GetThis());
            ServletDispatch::ptr getServletDispatch()const{return m_dispatch;}
            void setServletDispatch(ServletDispatch::ptr v){m_dispatch = v;}
        protected:
           virtual void handleClient(Socket::ptr client) override;
        private:
            bool m_isKeepalive;
            ServletDispatch::ptr m_dispatch;
        };
    }
}
#endif //HH_HTTP_SERVER_H
