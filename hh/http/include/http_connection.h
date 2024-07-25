//
// Created by 35148 on 2024/7/25.
//

#ifndef HH_HTTP_CONNECTION_H
#define HH_HTTP_CONNECTION_H
#include "socket_stream.h"
#include "http_parser.h"
namespace hh{
    namespace http{
        class HttpConnection : public SocketStream{
        public:
            typedef std::shared_ptr<HttpConnection> ptr;
            HttpConnection(Socket::ptr sock, bool owner = true);
            HttpResponse::ptr recvResponse();
            int sendRequest(HttpRequest::ptr rsp);
        };
    }
}
#endif //HH_HTTP_CONNECTION_H
