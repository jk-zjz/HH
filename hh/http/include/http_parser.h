//
// Created by 35148 on 2024/7/12.
//

#ifndef HH_HTTP_PARSER_H
#define HH_HTTP_PARSER_H
#include "http11_parser.h"
#include "httpclient_parser.h"
#include "http.h"
namespace hh{
    namespace http{
        class HttpRequestParser {
        public:
            typedef std::shared_ptr<HttpRequestParser> ptr;
            HttpRequestParser();
            int isFinish();
            int isError();
            size_t execute(char *data, size_t len);
            HttpRequest::ptr getData() const { return m_data; }
            void setError(int v) { m_error = v; }
            uint64_t getContentLength() const;
        private:
            http_parser m_parser;
            HttpRequest::ptr m_data;
            // 1000 请求错误
            // 1001 请求版本不支持
            // 1002 请求头错误
            int m_error;
        };
        class HttpResponseParser {
        public:
            typedef std::shared_ptr<HttpResponseParser> ptr;
            HttpResponseParser();
            int isFinish();
            int isError();
            size_t execute(char *data, size_t len);
            HttpResponse::ptr getData() const { return m_data; }
            void setError(int v) { m_error = v; }
            uint64_t getContentLength() const;
        private:
            httpclient_parser m_parser;
            HttpResponse::ptr m_data;
            int m_error;
        };
    }
}

#endif //HH_HTTP_PARSER_H
