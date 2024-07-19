//
// Created by 35148 on 2024/7/19.
//

#ifndef HH_HTTP_SERVER_H
#define HH_HTTP_SERVER_H
#include "tcp_server.h"
namespace hh{
    namespace http{
        class HttpServer : public TcpServer{
        public:
            typedef  std::shared_ptr<TcpServer> ptr;
            HttpServer(bool keepalive = false,
                       hh::IOManager *worker = hh::IOManager::GetThis(),
                       hh::IOManager *accept_worker = hh::IOManager::GetThis());
        protected:
           virtual void handleClient(Socket::ptr client) override;
        private:
            bool m_isKeepalive;
        };
    }
}
#endif //HH_HTTP_SERVER_H
