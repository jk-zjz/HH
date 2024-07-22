//
// Created by 35148 on 2024/7/19.
//
#include "http_server.h"
#include "http_session.h"
#include "log.h"
namespace hh {
    namespace http{
        static hh::Logger::ptr g_logger = HH_LOG_NAME("system");
        HttpServer::HttpServer(bool keepalive,
                   hh::IOManager *worker,
                   hh::IOManager *accept_worker):
                TcpServer(worker,accept_worker),
                m_isKeepalive(keepalive),
                m_dispatch(new ServletDispatch()){
        }
        void HttpServer::handleClient(Socket::ptr client){
            HttpSession::ptr session(new HttpSession(client));
            do{
                auto req = session->recvRequest();
                if(!req){
                    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)
                    <<"recv http request fail erron = "<<errno<<" errstr = "
                    <<strerror(errno)<<" client = "<<*client;
                    break;
                }
                HttpResponse::ptr rsp(new HttpResponse(req->getVersion(),
                                                       req->isClose() || !m_isKeepalive));
                m_dispatch->handle(req,rsp,session);
//                rsp->setHeader("Server","hh/1.0.0");
//                rsp->setBody("hello hh");
                session->sendResponse(rsp);
            } while (m_isKeepalive);
            session->close();
        }
    }
}
