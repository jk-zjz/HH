//
// Created by 35148 on 2024/7/10.
//

#include "http.h"
namespace hh {
    namespace http {
        // 字符串转请求方法
        HttpMethod stringToHttpMethod(const std::string &m) {
#define XX(num, name, string) \
            if(strcmp(m.c_str(), #string) == 0){ \
                return HttpMethod::name;\
            }                 \
            HTTP_METHOD_MAP(XX);
#undef XX
            return HttpMethod::INVALID_METHOD;
        }

        // 指针转请求方法
        HttpMethod CharsToHttpMethod(const char *m){
#define XX(num, name, string) \
            if(strcmp(m, #string) == 0){ \
                return HttpMethod::name;\
                }             \
                HTTP_METHOD_MAP(XX);
#undef XX
            return HttpMethod::INVALID_METHOD;
        }

        static const char *s_method_string[] = {
#define XX(num, name, string) #string,
                HTTP_METHOD_MAP(XX)
#undef XX
        };

        // 请求方法转字符串
        const char *httpMethodToString(const HttpMethod &m) {
            uint32_t index = (uint32_t) m;
            if (index >= (sizeof(s_method_string) / sizeof(s_method_string[0]))) {
                return "<unknown>";
            }
            return s_method_string[index];
        }

        // 状态码转字符串
        const char *httpStatusToString(const HttpStatus &s) {
            switch (s) {
#define XX(code, name, msg) \
                case HttpStatus::name: \
                    return #msg;
                HTTP_STATUS_MAP(XX)
#undef XX
                default:
                    return "<unknown>";
            }
        }
        HttpRequest::HttpRequest(uint8_t version , bool close):
        m_method(HttpMethod::GET),
        m_close(close),
        m_version(version),
        m_path("/"){
        }
        void HttpRequest::setHeader(const std::string &key, const std::string &val){
            m_headers[key] = val;
        }

        void HttpRequest::setParam(const std::string &key, const std::string &val){
            m_params[key] = val;
        }

        void HttpRequest::setCookie(const std::string &key, const std::string &val){
            m_cookies[key] = val;
        }

        std::string HttpRequest::getHeader(const std::string &key, const std::string &default_value) const{
            auto it = m_headers.find(key);
            return it == m_headers.end() ? default_value : it->second;
        }

        std::string HttpRequest::getParam(const std::string &key, const std::string &default_value) const{
            auto it = m_params.find(key);
            return it == m_params.end() ? default_value : it->second;
        }

        std::string HttpRequest::getCookie(const std::string &key, const std::string &default_value) const{
            auto it = m_cookies.find(key);
            return it == m_cookies.end() ? default_value : it->second;
        }
        void HttpRequest::delHeader(const std::string &key) {
            m_headers.erase(key);
        }

        void HttpRequest::delParam(const std::string &key) {
            m_params.erase(key);
        }

        void HttpRequest::delCookie(const std::string &key) {
            m_cookies.erase(key);
        }
        bool HttpRequest::hasHeader(const std::string &key, std::string *value){
            if(m_headers.find(key) != m_headers.end()){
                *value = m_headers[key];
                return true;
            }
            return false;
        }
        bool HttpRequest::hasParam(const std::string &key,std::string *value){
            if(m_params.find(key) != m_params.end()){
                *value = m_params[key];
                return true;
            }
            return false;
        }
        bool HttpRequest::hasCookie(const std::string &key,std::string *value){
            if(m_cookies.find(key) != m_cookies.end()){
                *value = m_cookies[key];
                return true;
            }
            return false;
        }

        std::ostream &HttpRequest::dump(std::ostream &os) {
            os<<httpMethodToString(m_method)<<" "
            <<m_path
            <<(m_query.empty()?"":"?")<<m_query
            <<(m_fragment.empty()?"":"#")<<m_fragment<<" "
            <<"HTTP/"<<(uint32_t)(m_version>>4)<<"."<<(uint32_t)(m_version&0x0f)<<"\r\n";
            os<<"connection:"<<(m_close?"closed":"keep-alive")<<"\r\n";

            for(auto &i : m_headers){
                if(strcasecmp(i.first.c_str(),"connection")){
                    os<<i.first<<":"<<i.second<<"\r\n";
                }
            }
            if(!m_body.empty()){
                os<<"content-length:"<<m_body.size()<<"\r\n\r\n"<<m_body;
            }else{
                os<<"\r\n";
            }
            return os;
        }

        HttpResponse::HttpResponse(uint8_t version, bool close):
        m_version(version),
        m_close(close),
        m_status(HttpStatus::OK){
        }
        void HttpResponse::setHeader(const std::string &key, const std::string &val){
            m_headers[key] = val;
        }
        std::string HttpResponse::getHeader(const std::string &key, const std::string &default_value)const {
            auto it = m_headers.find(key);
            return it == m_headers.end() ? default_value : it->second;
        };
        void HttpResponse::delHeader(const std::string &key){
            m_headers.erase(key);
        }
        bool HttpResponse::hasHeader(const std::string &key, std::string *value){
            if(m_headers.find(key) != m_headers.end()){
                *value = m_headers[key];
                return true;
            }
            return false;
        }
        std::ostream &HttpResponse::dump(std::ostream &os){
            os<<"HTTP/"<<(uint32_t)(m_version>>4)<<"."<<(uint32_t)(m_version&0x0f)<<" "
            <<(uint32_t)m_status<<" "
            <<(m_reason.empty()?httpStatusToString(m_status):m_reason)<<"\r\n";
            os<<"connection:"<<(m_close?"closed":"keep-alive")<<"\r\n";
            for(auto &i : m_headers){
                if(strcasecmp(i.first.c_str(),"connection") != 0){
                    os<<i.first<<":"<<i.second<<"\r\n";
                }
            }
            if(!m_body.empty()){
                os<<"content-length:"<<m_body.size()<<"\r\n\r\n"<<m_body;
            }else{
                os<<"\r\n";
            }
            return os;
        };
    }
}