//
// Created by 35148 on 2024/7/25.
//
#include "http_connection.h"
#include "iomanage.h"
#include "log.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
void test(){
//     hh::IPAddress::ptr ptr = hh::Address::lookupAnyIPAddress("www.sylar.top:80");
//     if(!ptr){
//         HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"error lookupAnyIPAddress fail";
//     }
//     std::cout<<ptr->toString()<<std::endl;
//     hh::Socket::ptr sock = hh::Socket::CreateTCP(ptr);
//     if(!sock->connect(ptr)){
//         HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"error connect fail";
//         return ;
//     }
//     hh::http::HttpConnection::ptr conn(new hh::http::HttpConnection(sock));
//     hh::http::HttpRequest::ptr req(new hh::http::HttpRequest);
//     req->setPath("/blog/");
//     req->setHeader("Host","www.sylar.top");
//     std::cout<<"req:"<<req->toString()<<std::endl;
//    conn->sendRequest(req);
//    hh::http::HttpResponse::ptr sharedPtr = conn->recvResponse();
//    if(sharedPtr){
////        std::cout<<"res:"<<sharedPtr->toString()<<std::endl;
//    }
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<"===============================";
    const hh::http::HttpResult::ptr &get = hh::http::HttpConnection::DoGet("https://www.baidu.com",300);
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<(get->response?get->response->toString():"error")<<std::endl;
}
int main(int args,char ** argv){
    hh::IOManager iom(2);
    iom.schedule(test);
    return 0;
}