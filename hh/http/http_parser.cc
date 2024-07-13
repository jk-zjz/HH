//
// Created by 35148 on 2024/7/12.
//
#include "http_parser.h"
#include "log.h"
#include "config.h"
namespace hh {
    namespace http {
        static hh::Logger::ptr g_logger = HH_LOG_NAME("system");
        static hh::ConfigVar<uint64_t>::ptr g_http_request_buffer_size =
                hh::Config::Lookup("http.request.buffer_size",(uint64_t)4*1024,"http request buffer size");

        static hh::ConfigVar<uint64_t>::ptr g_http_response_max_body_size =
                hh::Config::Lookup("http.response.max_body_size",(uint64_t)64*1024*1024,"http response max body size");

        static uint64_t s_http_request_buffer_size = 0;
        static uint64_t s_http_response_max_body_size = 0;

        struct _RequestSizeInit {
            _RequestSizeInit(){
                s_http_request_buffer_size = g_http_request_buffer_size->getValue();
                s_http_response_max_body_size = g_http_response_max_body_size->getValue();
                g_http_request_buffer_size->addOcb(
                        [](const uint64_t& old_value, const uint64_t& new_value){
                            s_http_request_buffer_size = new_value;
                        });
                g_http_response_max_body_size->addOcb(
                        [](const uint64_t& old_value, const uint64_t& new_value){
                            s_http_response_max_body_size = new_value;
                        });
            }
        };
        static _RequestSizeInit s_init;
        void on_request_method(void *data, const char *at, size_t length){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            HttpMethod m = CharsToHttpMethod(at);
            if(m == HttpMethod::INVALID_METHOD){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)<<"invalid http method:"<<std::string(at,length);
                parser->setError(1000);
                return;
            }
            parser->getData()->setMethod(m);
        }
        void on_request_fragment(void *data, const char *at, size_t length){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->getData()->setPath(std::string(at,length));
        }
        void on_request_path(void *data, const char *at, size_t length){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->getData()->setPath(std::string(at,length));
        }
        void on_request_query(void *data, const char *at, size_t length){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->getData()->setQuery(std::string(at,length));
        }
        void on_request_version(void *data, const char *at, size_t length){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            if(strncmp(at,"HTTP/1.0",length)==0){
                parser->getData()->setVersion(0x10);
            }else if(strncmp(at,"HTTP/1.1",length)==0){
                parser->getData()->setVersion(0x11);
            }else{
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)<<"invalid http version:"<<std::string(at,length);
                parser->setError(1001);
            }
        }
        void on_request_field(void *data, const char *field, size_t flen, const char *value, size_t vlen){
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            if(flen == 0){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)<<"invalid http field";
                parser->setError(1002);
                return;
            }
            parser->getData()->setHeader(std::string(field,flen),std::string(value,vlen));
        }
        void on_request_header(void *data, const char *at, size_t length){

        }
        void on_request_uri(void *data, const char *at, size_t length){
        }

        HttpRequestParser::HttpRequestParser():
        m_error(0){
            http_parser_init(&m_parser);
            m_data.reset(new HttpRequest);

            m_parser.http_field = on_request_field;
            // 获取请求地址
            m_parser.request_uri = on_request_uri;
            // 获取请求方法
            m_parser.request_method = on_request_method;
            // 获取请求片段
            m_parser.fragment = on_request_fragment;
            // 获取请求路径
            m_parser.request_path = on_request_path;
            // 获取请求参数
            m_parser.query_string = on_request_query;
            m_parser.header_done = on_request_header;
            // 获取请求版本
            m_parser.http_version = on_request_version;

            m_parser.data = this;
        }
        int HttpRequestParser::isFinish(){
            return http_parser_finish(&m_parser);
        };
        int HttpRequestParser::isError(){
            return m_error || http_parser_has_error(&m_parser);
        };
        size_t HttpRequestParser::execute(char *data, size_t len){
            size_t data_size = strlen(data);
            size_t offset = http_parser_execute(&m_parser,data,len,0);
            memmove(data,data+offset,len-offset);
            std::string tmp(data,data_size-offset);
            m_data->setBody(tmp);
            return offset;
        }

        uint64_t HttpRequestParser::getContentLength() const {
            return m_data->getHeaderAs<uint64_t>("Content-Length",0);
        }

        void on_response_field(void *data, const char *field, size_t flen, const char *value, size_t vlen){
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            if(flen == 0){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)<<"invalid http field";
                parser->setError(1002);
                return;
            }
            parser->getData()->setHeader(std::string(field,flen),std::string(value,vlen));
        }
        void on_response_reason(void *data, const char *at, size_t length){
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            parser->getData()->setReason(std::string(at,length));
        }
        void on_response_status(void *data, const char *at, size_t length){
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            HttpStatus s = (HttpStatus)boost::lexical_cast<int>(std::string(at,length));
            parser->getData()->setStatus(s);
        }
        void on_response_version(void *data, const char *at, size_t length){
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            if(strncmp(at,"HTTP/1.0",length)==0){
                parser->getData()->setVersion(0x10);
            }else if(strncmp(at,"HTTP/1.1",length)==0){
                parser->getData()->setVersion(0x11);
            }else{
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::WARN)<<"invalid http version:"<<std::string(at,length);
                parser->setError(1001);
            }
        }
        void on_response_header(void *data, const char *at, size_t length){

        }
        void on_response_last(void *data, const char *at, size_t length){

        }
        void on_response_chunk(void *data, const char *at, size_t length){

        }

        HttpResponseParser::HttpResponseParser():m_error(0){
            httpclient_parser_init(&m_parser);
            m_data.reset(new HttpResponse);
            m_parser.http_field = on_response_field;
            m_parser.reason_phrase = on_response_reason;
            m_parser.status_code = on_response_status;
            m_parser.http_version = on_response_version;
            m_parser.header_done = on_response_header;
            m_parser.last_chunk =  on_response_last;
            m_parser.chunk_size = on_response_chunk;

            m_parser.data = this;
        };
        int HttpResponseParser::isFinish(){
            return httpclient_parser_finish(&m_parser);
        };
        int HttpResponseParser::isError(){
            return m_error || httpclient_parser_has_error(&m_parser);
        }

        size_t HttpResponseParser::execute(char *data, size_t len) {
            size_t data_size = strlen(data);
            size_t offset = httpclient_parser_execute(&m_parser,data,len,0);
            memmove(data,data+offset,len-offset);
            std::string tmp(data,data_size-offset);
            m_data->setBody(tmp);
            return offset;
        }

        uint64_t HttpResponseParser::getContentLength() const {
            return m_data->getHeaderAs<uint64_t>("Content-Length",0);
        };


    }
}
