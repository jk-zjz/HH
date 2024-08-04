//
// Created by 35148 on 2024/7/25.
//

#ifndef HH_HTTP_CONNECTION_H
#define HH_HTTP_CONNECTION_H
#include "socket_stream.h"
#include "http_parser.h"
#include "uri.h"
#include "thread.h"
#include <list>
#include "util.h"
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
                BIND_FAIL =7,
                POOL_GET_CONNECTION = 8,
                POOL_INVALID_CONNECTION = 9
            };
            HttpResult(Error _result, const std::string &_error, HttpResponse::ptr _response)
            :result(_result),error(_error),response(_response){
            }
            Error result;
            std::string error;
            HttpResponse::ptr response;
            std::string toString();
        };
        class HttpConnectionPool;

        class HttpConnection : public SocketStream{
        public:
            friend class HttpConnectionPool;
            typedef std::shared_ptr<HttpConnection> ptr;
            /**
             * 获取请求
             * @param url
             * @param timeout_ms
             * @param headers
             * @param body
             * @return
             */
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
            ~HttpConnection();
            int sendRequest(HttpRequest::ptr rsp);
        private:
            uint64_t m_createTime = 0;
            uint64_t m_request = 0;
        };
        class HttpConnectionPool{
        public:
            typedef std::shared_ptr<HttpConnectionPool> ptr;
            typedef Mutex MutexType;
            HttpConnectionPool(const std::string &host,
                               const std::string& vhost,
                               uint32_t port,
                               uint32_t max_size = 100,
                               uint32_t max_alive_time = 300000,
                               uint32_t max_request = 10);
            HttpConnection::ptr getConnection();

            HttpResult::ptr doGet(const std::string &url,
                                   uint64_t timeout_ms,
                                   const std::map<std::string, std::string> &headers = {},
                                   const std::string &body = "");
            HttpResult::ptr doGet(Uri::ptr uri,
                                   uint64_t timeout_ms,
                                   const std::map<std::string, std::string> &headers = {},
                                   const std::string &body = "");
            HttpResult::ptr doPost(Uri::ptr uri,
                                   uint64_t timeout_ms,
                                   const std::map<std::string, std::string> &headers = {},
                                   const std::string &body = "");
            HttpResult::ptr doPost(const std::string &url,
                                   uint64_t timeout_ms,
                                   const std::map<std::string, std::string> &headers = {},
                                   const std::string &body = "");
            HttpResult::ptr doRequest(HttpMethod method,
                                             const std::string &url,
                                             uint64_t timeout_ms,
                                             const std::map<std::string, std::string> &headers = {},
                                             const std::string &body = "");
            HttpResult::ptr doRequest(HttpMethod method,
                                             const Uri::ptr uri,
                                             uint64_t timeout_ms,
                                             const std::map<std::string, std::string> &headers = {},
                                             const std::string &body = "");
            HttpResult::ptr doRequest(HttpRequest::ptr rsp,
                                             uint64_t timeout_ms);
        private:
            static void ReleasePtr(HttpConnection*ptr,HttpConnectionPool * pool);
        private:
            std::string m_host;         // 服务器地址
            std::string m_vhost;        // 虚拟主机
            uint32_t m_port;            // 端口
            uint32_t m_max_size;        // 最大连接数
            uint32_t m_max_alive_time;  // 最大空闲时间
            uint32_t m_max_request;     // 最大请求数

            MutexType m_mutex;
            std::list<HttpConnection*> m_conns; // 连接池
            std::atomic<int32_t> m_total = {0};     // 总连接数
        };
    }
}
#endif //HH_HTTP_CONNECTION_H
