//
// Created by 35148 on 2024/7/13.
//


//#include <string>
//#include "http_parser.h"
//#include "log.h"
//
//static hh::Logger::ptr g_logger = HH_LOG_ROOT();
//
//const char test_request_data[] = "POST / HTTP/1.1\r\n"
//                                 "Host: www.sylar.top\r\n"
//                                 "Content-Length: 10\r\n\r\n"
//                                 "1234567890";
//
//void test_request() {
//    hh::http::HttpRequestParser parser;
//    std::string tmp = test_request_data;
//    parser.execute(&tmp[0], tmp.size());
////    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
////                                                    << "has_error=" << parser.isError()
////                                                    << " is_finished=" << parser.isFinish()
////                                                    << " total=" << tmp.size()
////                                                    << " content_length=" << parser.getContentLength();
//    std::cout << parser.getData()->toString();
//}
//
//const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
//                                  "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
//                                  "Server: Apache\r\n"
//                                  "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
//                                  "ETag: \"51-47cf7e6ee8400\"\r\n"
//                                  "Accept-Ranges: bytes\r\n"
//                                  "Content-Length: 81\r\n"
//                                  "Cache-Control: max-age=86400\r\n"
//                                  "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
//                                  "Connection: Close\r\n"
//                                  "Content-Type: text/html\r\n\r\n"
//                                  "<html>\r\n"
//                                  "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
//                                  "</html>\r\n";
//
//void test_response() {
//    hh::http::HttpResponseParser parser;
//    std::string tmp = test_response_data;
//    parser.execute(&tmp[0], tmp.size());
////    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
////                                                    << " has_error=" << parser.isError()
////                                                    << " is_finished=" << parser.isFinish()
////                                                    << " total=" << tmp.size()
////                                                    << " content_length=" << parser.getContentLength()
////                                                    << " tmp[s]=" << tmp[s];
//
//    std::cout << parser.getData()->toString();
//}
//
//int main(int argc, char** argv) {
//    test_request();
//    std::cout<<"-------------------------"<<std::endl;
//    test_response();
//    return 0;
//}


#include "http_parser.h"
#include <iostream>
#include "log.h"
static  hh::Logger::ptr g_logger = HH_LOG_ROOT();
const char test_request_data[] = "POST / HTTP/1.1\r\n"
                                 "Host: www.sylar.top\r\n"
                                 "Content-Length: 10\r\n\r\n"
                                 "1234567890";

void test_request() {
    hh::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
                                                    << "has_error=" << parser.isError()
                                                    << " is_finished=" << parser.isFinish()
                                                    << " total=" << tmp.size()
                                                    << " content_length=" << parser.getContentLength();
    tmp.resize(tmp.size() - s);
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << parser.getData()->toString();
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << tmp;
}

const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
                                  "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
                                  "Server: Apache\r\n"
                                  "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
                                  "ETag: \"51-47cf7e6ee8400\"\r\n"
                                  "Accept-Ranges: bytes\r\n"
                                  "Content-Length: 81\r\n"
                                  "Cache-Control: max-age=86400\r\n"
                                  "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
                                  "Connection: Close\r\n"
                                  "Content-Type: text/html\r\n\r\n"
                                  "<html>\r\n"
                                  "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
                                  "</html>\r\n";

void test_response() {
    hh::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
                                                    << " has_error=" << parser.isError()
                                                    << " is_finished=" << parser.isFinish()
                                                    << " total=" << tmp.size()
                                                    << " content_length=" << parser.getContentLength()
                                                    << " tmp[s]=" << tmp[s];

    tmp.resize(tmp.size() - s);

    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << parser.getData()->toString();
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<< tmp;
}

void test_request_parser(){
    std::string buff = "GET /api/sort?id=1 HTTP/1.1\r\n"
                       "Host: www.baidu.com\r\n"
                       "Connection: keep-alive\r\n"
                       "Upgrade-Insecure-Requests: 1\r\n"
                       "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n\r\n"
                       "{\"id\":1,\"jsonrpc\":2.0,\"method\":\"sum\",\"params\":[1,2,4]}\r\n";
    hh::http::HttpRequestParser parser;
    parser.execute(&buff[0],buff.size());
    std::cout<<parser.getData()->toString();
}
void test_rsponse_parser(){
    std::string buff = "HTTP/1.1 200 OK\r\n"
                       "Connection: keep-alive\r\n"
                       "xxx:1\r\n"
                       "Content-Length: 20\r\n\r\n";
    hh::http::HttpResponseParser parser;
    parser.execute(&buff[0],buff.size());
    std::cout<<parser.getData()->toString();
}
int main(int argc, char **argv){
    test_request_parser();
    test_rsponse_parser();
    test_request();
    std::cout<<"-------------------------"<<std::endl;
    test_response();
    return 0;
}

/*
 * #include <string>
#include "http_parser.h"
#include "log.h"

static hh::Logger::ptr g_logger = HH_LOG_ROOT();

const char test_request_data[] = "POST / HTTP/1.1\r\n"
                                 "Host: www.sylar.top\r\n"
                                 "Content-Length: 10\r\n\r\n"
                                 "1234567890";

void test_request() {
    hh::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
                              << "has_error=" << parser.isError()
                              << " is_finished=" << parser.isFinish()
                              << " total=" << tmp.size()
                              << " content_length=" << parser.getContentLength();
    tmp.resize(tmp.size() - s);
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << parser.getData()->toString();
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << tmp;
}

const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
                                  "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
                                  "Server: Apache\r\n"
                                  "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
                                  "ETag: \"51-47cf7e6ee8400\"\r\n"
                                  "Accept-Ranges: bytes\r\n"
                                  "Content-Length: 81\r\n"
                                  "Cache-Control: max-age=86400\r\n"
                                  "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
                                  "Connection: Close\r\n"
                                  "Content-Type: text/html\r\n\r\n"
                                  "<html>\r\n"
                                  "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
                                  "</html>\r\n";

void test_response() {
    hh::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "execute rt=" << s
                              << " has_error=" << parser.isError()
                              << " is_finished=" << parser.isFinish()
                              << " total=" << tmp.size()
                              << " content_length=" << parser.getContentLength()
                              << " tmp[s]=" << tmp[s];

    tmp.resize(tmp.size() - s);

    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << parser.getData()->toString();
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<< tmp;
}

int main(int argc, char** argv) {
    test_request();
   std::cout<<"-------------------------"<<std::endl;
    test_response();
    return 0;
}
 *
 *
 * */