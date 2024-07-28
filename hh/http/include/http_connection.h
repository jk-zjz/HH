//
// Created by 35148 on 2024/7/25.
//

#ifndef HH_HTTP_CONNECTION_H
#define HH_HTTP_CONNECTION_H
#include "socket_stream.h"
#include "http_parser.h"
#include "uri.h"

namespace hh{
    namespace http{
        // 结果集 发送的结果
        struct HttpResult{
            typedef std::shared_ptr<HttpResult> ptr;
            enum class Error{
                OK = 0,
                INVALID_URL = 1,
                INVALID_HOST =2,
                CONNECT_FAIL = 3,
                SEND_CLOSE_BY_PEER =4,
                SEND_SOCKET_ERROR = 5,
                TIMEOUT = 6,
                BIND_FAIL =7
            };
            HttpResult(Error _result, const std::string &_error, HttpResponse::ptr _response)
            :result(_result),error(_error),response(_response){
            }
            Error result;
            std::string error;
            HttpResponse::ptr response;
        };
        class HttpConnection : public SocketStream{
        public:
            typedef std::shared_ptr<HttpConnection> ptr;
            static HttpResult::ptr DoGet(const std::string &url,
                                         uint64_t timeout_ms,
                                         const std::map<std::string, std::string> &headers = {},
                                         const std::string &body = "");
            static HttpResult::ptr DoGet(const Uri::ptr uri,
                                         uint64_t timeout_ms,
                                         const std::map<std::string, std::string> &headers = {},
                                         const std::string &body = "");
            static HttpResult::ptr DoPost(const std::string &url,
                                         uint64_t timeout_ms,
                                         const std::map<std::string, std::string> &headers = {},
                                         const std::string &body = "");
            static HttpResult::ptr DoPost(const Uri::ptr uri,
                                         uint64_t timeout_ms,
                                         const std::map<std::string, std::string> &headers = {},
                                         const std::string &body = "");
            static HttpResult::ptr DoRequest(HttpMethod method,
                                             const std::string &url,
                                             uint64_t timeout_ms,
                                             const std::map<std::string, std::string> &headers = {},
                                             const std::string &body = "");
            static HttpResult::ptr DoRequest(HttpMethod method,
                                             const Uri::ptr uri,
                                             uint64_t timeout_ms,
                                             const std::map<std::string, std::string> &headers = {},
                                             const std::string &body = "");
            static HttpResult::ptr DoRequest(HttpRequest::ptr rsp,
                                             Uri::ptr uri,
                                             uint64_t timeout_ms);

            HttpConnection(Socket::ptr sock, bool owner = true);
            HttpResponse::ptr recvResponse();
            int sendRequest(HttpRequest::ptr rsp);
        };
    }
}
#endif //HH_HTTP_CONNECTION_H
