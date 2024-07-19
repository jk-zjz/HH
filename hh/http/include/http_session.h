//
// Created by 35148 on 2024/7/19.
//

#ifndef HH_HTTP_SESSION_H
#define HH_HTTP_SESSION_H
#include "socket_stream.h"
#include "http_parser.h"
#include <memory>
namespace hh{
    namespace http{
        class HttpSession : public SocketStream{
        public:
            typedef std::shared_ptr<HttpSession> ptr;

            HttpSession(Socket::ptr sock, bool owner = true);
            HttpRequest::ptr recvRequest();
            int sendResponse(HttpResponse::ptr rsp);
        };
    }
}
#endif //HH_HTTP_SESSION_H
