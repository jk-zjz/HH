//
// Created by 35148 on 2024/7/25.
//
#include "http_connection.h"
#include "log.h"
#include <functional>
namespace hh {
    namespace http {
        static Logger::ptr g_logger = HH_LOG_NAME("system");

        HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
                : SocketStream(sock, owner) {
            m_createTime = hh::GetCurrentMS();
        }

        HttpResponse::ptr HttpConnection::recvResponse() {
            HttpResponseParser::ptr parser(new HttpResponseParser);
            uint64_t buff_size = HttpResponseParser::getHttpResponseBufferSize();
            //uint64_t buff_size = 100;
            std::shared_ptr<char> buffer(
                    new char[buff_size + 1], [](char *ptr) {
                        delete[] ptr;
                    });
            char *data = buffer.get();
            int offset = 0;
            do {
                int len = read(data + offset, buff_size - offset);
                if (len <= 0) {
                    close();
                    return nullptr;
                }
                len += offset;
                data[len] = '\0';
                size_t nparse = parser->execute(data, len, false);
                if (parser->isError()) {
                    close();
                    return nullptr;
                }
                offset = len - nparse;
                if (offset == (int) buff_size) {
                    close();
                    return nullptr;
                }
                if (parser->isFinish()) {
                    break;
                }
            } while (true);
            auto &client_parser = parser->getClientParser();
            std::string body;
            if (client_parser.chunked) {
                int len = offset;
                do {
                    bool begin = true;
                    do {
                        if (!begin || len == 0) {
                            int rt = read(data + len, buff_size - len);
                            if (rt <= 0) {
                                close();
                                return nullptr;
                            }
                            len += rt;
                        }
                        data[len] = '\0';
                        size_t nparse = parser->execute(data, len, true);
                        if (parser->isError()) {
                            close();
                            return nullptr;
                        }
                        len -= nparse;
                        if (len == (int) buff_size) {
                            close();
                            return nullptr;
                        }
                        begin = false;
                    } while (!parser->isFinish());
                    //len -= 2;

                    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "content_len=" << client_parser.content_len;
                    if (client_parser.content_len + 2 <= len) {
                        body.append(data, client_parser.content_len);
                        memmove(data, data + client_parser.content_len + 2, len - client_parser.content_len - 2);
                        len -= client_parser.content_len + 2;
                    } else {
                        body.append(data, len);
                        int left = client_parser.content_len - len + 2;
                        while (left > 0) {
                            int rt = read(data, left > (int) buff_size ? (int) buff_size : left);
                            if (rt <= 0) {
                                close();
                                return nullptr;
                            }
                            body.append(data, rt);
                            left -= rt;
                        }
                        body.resize(body.size() - 2);
                        len = 0;
                    }
                } while (!client_parser.chunks_done);
            } else {
                int64_t length = parser->getContentLength();
                if (length > 0) {
                    body.resize(length);

                    int len = 0;
                    if (length >= offset) {
                        memcpy(&body[0], data, offset);
                        len = offset;
                    } else {
                        memcpy(&body[0], data, length);
                        len = length;
                    }
                    length -= offset;
                    if (length > 0) {
                        if (readFixSize(&body[len], length) <= 0) {
                            close();
                            return nullptr;
                        }
                    }
                }
            }
            parser->getData()->setBody(body);
            return parser->getData();
        }

        int HttpConnection::sendRequest(HttpRequest::ptr rsp) {
            std::string str = rsp->toString();
            return writeFixSize(str.c_str(), str.size());
        }

        HttpResult::ptr HttpConnection::DoGet(const std::string &url, uint64_t timeout_ms,
                                              const std::map<std::string, std::string> &headers,
                                              const std::string &body) {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) {
                HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "invalid url " << url;
                return std::make_shared<HttpResult>(HttpResult::Error::INVALID_URL,
                                                    "invalid url " + url, nullptr);
            }
            return DoGet(uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoGet(const Uri::ptr uri, uint64_t timeout_ms,
                                              const std::map<std::string, std::string> &headers,
                                              const std::string &body) {

            return DoRequest(HttpMethod::GET, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoPost(const std::string &url, uint64_t timeout_ms,
                                               const std::map<std::string, std::string> &headers,
                                               const std::string &body) {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) {
                HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "invalid url " << url;
                return std::make_shared<HttpResult>(HttpResult::Error::INVALID_URL,
                                                    "invalid url " + url, nullptr);
            }
            return DoPost(uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoPost(const Uri::ptr uri, uint64_t timeout_ms,
                                               const std::map<std::string, std::string> &headers,
                                               const std::string &body) {
            return DoRequest(HttpMethod::POST, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method, const std::string &url, uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body) {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) {
                HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "invalid url " << url;
                return std::make_shared<HttpResult>(HttpResult::Error::INVALID_URL,
                                                    "invalid url " + url, nullptr);
            }
            return DoRequest(method, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method, const Uri::ptr uri, uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body) {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->setPath(uri->getPath());
            req->setQuery(uri->getQuery());
            req->setFragment(uri->getFragment());
            req->setMethod(method);
            bool has_host = false;
            for (auto &i: headers) {
                if (strcasecmp(i.first.c_str(), "Connection") == 0) {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0) {
                        req->setClose(false);
                    }
                    continue;
                }
                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0) {
                    has_host = !i.second.empty();
                }
                req->setHeader(i.first, i.second);
            }
            if (!has_host) {
                req->setHeader("Host", uri->getHost());
            }
            req->setBody(body);
            return DoRequest(req, uri, timeout_ms);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr req, Uri::ptr uri, uint64_t timeout_ms) {
//            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG)<<*req;
            Address::ptr addr = uri->createAddress();
            if (!addr) {
                return std::make_shared<HttpResult>(HttpResult::Error::INVALID_HOST,
                                                    "invalid host " + uri->getHost(), nullptr);
            }
            std::cout << *addr << std::endl;
            Socket::ptr sock = Socket::CreateTCP(addr);
            if (!sock->connect(addr)) {
                return std::make_shared<HttpResult>(HttpResult::Error::CONNECT_FAIL,
                                                    "connect fail " + uri->getHost(), nullptr);
            }
            sock->setRecvTimeout(timeout_ms);
            HttpConnection::ptr conn = std::make_shared<HttpConnection>(sock);
            int rt = conn->sendRequest(req);
            if (rt == 0) {
                return std::make_shared<HttpResult>(HttpResult::Error::SEND_CLOSE_BY_PEER,
                                                    "send request closed by peer " + addr->toString(), nullptr);
            }
            if (rt < 0) {
                return std::make_shared<HttpResult>(HttpResult::Error::SEND_SOCKET_ERROR,
                                                    "send request socket error " + addr->toString(), nullptr);
            }
            auto rsp = conn->recvResponse();
            if (!rsp) {
                return std::make_shared<HttpResult>(HttpResult::Error::TIMEOUT,
                                                    "recv response timeout" + addr->toString()
                                                    + "timeout_ms :" + std::to_string(timeout_ms), nullptr);
            }
            return std::make_shared<HttpResult>(HttpResult::Error::OK, "OK", rsp);
        }

        HttpConnectionPool::HttpConnectionPool(const std::string &host,
                                               uint32_t port, uint32_t max_size,
                                               uint32_t max_alive_time, uint32_t max_request) :
                m_host(host),
                m_port(port), m_max_size(max_size), m_max_alive_time(max_alive_time),
                m_max_request(max_request) {
        }

        HttpConnection::ptr HttpConnectionPool::getConnection() {
            // 当前时间
            uint64_t now_time = hh::GetCurrentMS();
            // 失败链接
            std::vector<HttpConnection*>invalid_conns;
            // 成功获取的链接
            HttpConnection* ptr = nullptr;
            MutexType::Lock lock(m_mutex);
            while(!m_conns.empty()){
                auto conn = *m_conns.begin();
                m_conns.pop_front();
                if(!conn->isConnected()){
                    invalid_conns.push_back(conn);
                    continue;
                }
                // 创建时间 + 最大存活时间 > 当前时间,已经无效
                if(conn->m_createTime + m_max_alive_time > now_time) {
                    invalid_conns.push_back(conn);
                    continue;
                }
                ptr = conn;
                break;
            }
            lock.unlock();
            for(auto i : invalid_conns) {
                delete i;
            }
            m_total -= invalid_conns.size();
            if(!ptr){
                // 没有链接了
                hh::Address::ptr addr = hh::Address::lookupAny(m_host);
                if(!addr){
                    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "invalid host " << m_host;
                    return nullptr;
                }
                Socket::ptr sock = hh::Socket::CreateTCP(addr);
                if(!sock){
                    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "connect " << m_host << ":" << m_port << " failed";
                    return nullptr;
                }
                if(!sock->connect(addr)){
                    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR) << "connect " << m_host << ":" << m_port << " failed";
                    return nullptr;
                }
                ptr = new HttpConnection(sock);
                ++m_total;
            }
            return HttpConnection::ptr(ptr,std::bind(&HttpConnectionPool::ReleasePtr,
                                                     std::placeholders::_1, this));
        }

        void HttpConnectionPool::ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool) {

        }
        HttpResult::ptr HttpConnectionPool::doGet(const std::string &url, uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body) {

            return hh::http::HttpResult::ptr();
        }

        HttpResult::ptr
        HttpConnectionPool::doGet(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers,
                                  const std::string &body) {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?") << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#") << uri->getFragment();
            return doGet(ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr
        HttpConnectionPool::doPost(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers,
                                   const std::string &body) {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?") << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#") << uri->getFragment();
            return doPost(ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::doPost(const std::string &url, uint64_t timeout_ms,
                                                   const std::map<std::string, std::string> &headers,
                                                   const std::string &body) {

            return hh::http::HttpResult::ptr();
        }

        HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method, const std::string &url, uint64_t timeout_ms,
                                                      const std::map<std::string, std::string> &headers,
                                                      const std::string &body) {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->setPath(url);
            req->setMethod(method);
            bool has_host = false;
            for (auto &i: headers) {
                if (strcasecmp(i.first.c_str(), "Connection") == 0) {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0) {
                        req->setClose(false);
                    }
                    continue;
                }
                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0) {
                    has_host = !i.second.empty();
                }
                req->setHeader(i.first, i.second);
            }
            if (!has_host) {
                if(!m_vhost.empty()){
                    req->setHeader("Host",m_vhost);
                }else{
                    req->setHeader("Host",m_host);
                }
            }
            req->setBody(body);
            return doRequest(req, timeout_ms);
        }

        HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method, const Uri::ptr uri, uint64_t timeout_ms,
                                                      const std::map<std::string, std::string> &headers,
                                                      const std::string &body) {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?") << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#") << uri->getFragment();
            return doRequest(method, ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr req,uint64_t timeout_ms) {
            auto conn =getConnection();
            if(!conn){
                return std::make_shared<HttpResult>(HttpResult::Error::POOL_GET_CONNECTION,
                                                    "pool host: "+m_host+"port: "+std::to_string(m_port), nullptr);
            }
            auto sock = conn->getSocket();
            if(!sock){
                return std::make_shared<HttpResult>(HttpResult::Error::POOL_INVALID_CONNECTION,
                                                    "pool host: "+m_host+"port: "+std::to_string(m_port), nullptr);
            }
            sock->setRecvTimeout(timeout_ms);
            int rt = conn->sendRequest(req);
            if (rt == 0) {
                return std::make_shared<HttpResult>(HttpResult::Error::SEND_CLOSE_BY_PEER,
                                                    "send request closed by peer " + m_host, nullptr);
            }
            if (rt < 0) {
                return std::make_shared<HttpResult>(HttpResult::Error::SEND_SOCKET_ERROR,
                                                    "send request socket error " + m_host, nullptr);
            }
            auto rsp = conn->recvResponse();
            if (!rsp) {
                return std::make_shared<HttpResult>(HttpResult::Error::TIMEOUT,
                                                    "recv response timeout" +m_host
                                                    + "timeout_ms :" + std::to_string(timeout_ms), nullptr);
            }
            return std::make_shared<HttpResult>(HttpResult::Error::OK, "OK", rsp);
        }
    }
}