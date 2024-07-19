//
// Created by 35148 on 2024/7/10.
//

#ifndef HH_HTTP_H
#define HH_HTTP_H

#include <string>
#include <memory>
#include <map>
#include <boost/lexical_cast.hpp>

namespace hh {
    namespace http {
        /* HttpRequest Methods */
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* WebDAV */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  /* subversion */                  \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  /* CalDAV */                      \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */  \
  XX(31, LINK,        LINK)         \
  XX(32, UNLINK,      UNLINK)       \
  /* icecast */                     \
  XX(33, SOURCE,      SOURCE)       \

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_HttpRequest,                     Bad HttpRequest)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, HttpRequest_TIMEOUT,                 HttpRequest Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_HttpRequest,             Misdirected HttpRequest)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_HttpRequestS,               Too Many HttpRequests)               \
  XX(431, HttpRequest_HEADER_FIELDS_TOO_LARGE, HttpRequest Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

        // http 请求宏
        enum class HttpMethod {
#define XX(num, name, string) name = num,
            HTTP_METHOD_MAP(XX)
#undef XX
            INVALID_METHOD
        };
        // http 状态码宏
        enum class HttpStatus {
#define XX(code, name, desc) name = code,
            HTTP_STATUS_MAP(XX)
#undef XX
        };

        // 字符串转请求方法
        HttpMethod stringToHttpMethod(const std::string &m);

        // 指针转请求方法
        HttpMethod CharsToHttpMethod(const char *m);

        // 请求方法转字符串
        const char * httpMethodToString(const HttpMethod &m);

        // 状态码转字符串
        const char * httpStatusToString(const HttpStatus &s);

        class CaseInsensitiveLess {
        public:
            bool operator()(const std::string &lhs, const std::string &rhs) const {
                return strcasecmp(lhs.c_str(), rhs.c_str())<0;
            }
        };
        /**
         * 获取返回获取成功，否则失败
         * @tparam T
         * @param m
         * @param key
         * @param value
         * @param def
         * @return
         */
        template<class MapType ,class T>
        bool checkGetAs(const MapType &m, const std::string &key, T &value, const T &def = T()) {
            auto it = m.find(key);
            if (it == m.end()) {
                value = def;
                return false;
            }
            try {
                value = boost::lexical_cast<T>(it->second);
                return true;
            } catch (...) {
                value = def;
            }
            return false;
        }

        /**
         * 通过返回值的方式获取
         * @tparam T
         * @param m
         * @param key
         * @param def
         * @return
         */
        template<class MapType ,class T>
        T getAs(const MapType &m, const std::string &key, const T &def = T()) {
            auto i = m.find(key);
            if (i == m.end()) {
                return def;
            }
            try {
                return boost::lexical_cast<T>(i->second);
            } catch (...) {
                return def;
            }
            return def;
        }
        class  HttpRequest{
        public:
            typedef std::shared_ptr<HttpRequest> ptr;
            typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

            HttpRequest(uint8_t version = 0x11, bool close = true);

            HttpMethod getMethod() const { return m_method; }

            void setMethod(const HttpMethod &v) { m_method = v; }

            uint8_t getVersion() const { return m_version; }

            void setVersion(const uint8_t &v) { m_version = v; }

            std::string getPath() const { return m_path; }

            void setPath(const std::string &v) { m_path = v; }

            std::string getQuery() const { return m_query; }

            void setQuery(const std::string &v) { m_query = v; }

            std::string getFragment() const { return m_fragment; }

            void setFragment(const std::string &v) { m_fragment = v; }

            std::string getBody() const { return m_body; }

            void setBody(const std::string &v) { m_body = v; }

            MapType &getHeaders() { return m_headers; }

            const MapType &getHeaders() const { return m_headers; }

            MapType &getParams() { return m_params; }

            const MapType &getParams() const { return m_params; }

            MapType &getCookies() { return m_cookies; }

            const MapType &getCookies() const { return m_cookies; }

            void setClose(bool v) { m_close = v; }

            bool isClose() const { return m_close; }

            // 设置
            void setHeader(const std::string &key, const std::string &val);

            void setParam(const std::string &key, const std::string &val);

            void setCookie(const std::string &key, const std::string &val);

            // 获取
            std::string getHeader(const std::string &key, const std::string &default_value = "") const;

            std::string getParam(const std::string &key, const std::string &default_value = "") const;

            std::string getCookie(const std::string &key, const std::string &default_value = "") const;

            // 删除
            void delHeader(const std::string &key);

            void delParam(const std::string &key);

            void delCookie(const std::string &key);

            bool hasHeader(const std::string &key, std::string *value);
            bool hasParam(const std::string &key,std::string *value);
            bool hasCookie(const std::string &key,std::string *value);

            std::ostream &dump(std::ostream &os)const;

            std::string toString()const;
            template<class T>
            bool checkGetHeader(const std::string &key, T &value, const T &def = T()) {
                return checkGetAs(m_headers, key, value, def);
            }
            template<class T>
            bool checkGetParam(const std::string &key, T &value, const T &def = T()) {
                return checkGetAs(m_params, key, value, def);
            }
            template<class T>
            bool checkGetCookie(const std::string &key, T &value, const T &def = T()) {
                return checkGetAs(m_cookies, key, value, def);
            }

            template<class T>
            T getHeaderAs(const std::string &key, const T &def = T()) {
                return getAs(m_headers, key, def);
            }
            template<class T>
            T getParamAs(const std::string &key, const T &def = T()) {
                return getAs(m_params, key, def);
            }
            template<class T>
            T getCookieAs(const std::string &key, const T &def = T()) {
                return getAs(m_cookies, key, def);
            }
        private:
        private:
            HttpMethod m_method;    // 请求方法
            bool m_close;           // 是否关闭
            uint8_t m_version;      // 版本
            std::string m_path;     // 路径
            std::string m_query;    // 查询
            std::string m_fragment; // 片段
            std::string m_body;     // 请求体
            MapType m_headers;      // 头
            MapType m_params;       // 参数
            MapType m_cookies;      // cookie
        };
        class HttpResponse {
        public:
            typedef std::shared_ptr<HttpResponse> ptr;
            typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;
            HttpResponse(uint8_t version = 0x11, bool close = true);

            HttpStatus getStatus() const { return m_status; }
            void setStatus(const HttpStatus &v) { m_status = v; }
            std::string getReason() const { return m_reason; }
            void setReason(const std::string &v) { m_reason = v; }
            std::string getBody() const { return m_body; }
            void setBody(const std::string &v) { m_body = v; }
            MapType &getHeaders() { return m_headers; }
            const MapType &getHeaders() const { return m_headers; }
            void setClose(bool v) { m_close = v; }
            bool isClose() const { return m_close; }

            void setVersion(const uint8_t &v) { m_version = v; }
            uint8_t getVersion() const { return m_version; }

            void setHeader(const std::string &key, const std::string &val);
            std::string getHeader(const std::string &key, const std::string &default_value = "") const;
            void delHeader(const std::string &key);

            bool hasHeader(const std::string &key, std::string *value);
            std::ostream &dump(std::ostream &os) const;
            std::string toString() const;
            template<class T>
            bool checkGetHeader(const std::string &key, T &value, const T &def = T()) {
                return checkGetAs(m_headers, key, value, def);
            }
            template<class T>
            T getHeaderAs(const std::string &key, const T &def = T()) {
                return getAs(m_headers, key, def);
            }

        private:
            uint8_t m_version;      // 版本
            bool m_close;           // 是否关闭
            HttpStatus m_status;    // 状态码
            std::string m_reason;   // 状态码,后部分
            std::string m_body;     // 请求体
            MapType m_headers;      // 头
        };
        std::ostream &operator<<(std::ostream &os, const HttpRequest &req);
        std::ostream &operator<<(std::ostream &os, const HttpResponse &rsp);
    }
}

#endif //HH_HTTP_H
