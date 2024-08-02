//
// Created by 35148 on 2024/7/25.
//
#include "http_connection.h"
#include "iomanage.h"
#include "log.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
void test2(){
    hh::http::HttpConnectionPool::ptr pool
    (new hh::http::HttpConnectionPool("www.sylar.top","",
                                      80,10,300*10,10));
    hh::IOManager::GetThis()->addTimer(1000,[pool](){
        auto it = pool->doGet("/",3000);
        if(it){
            HH_LOG_INFO(g_logger,it->toString());
        }
        },true);
}
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
//    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<"===============================";
//    std::map<std::string,std::string>head;
//    auto r = hh::http::HttpConnection::DoGet("http://www.sylar.top/blog/",1000);
//    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "result=" << (int)r->result
//                             << " error=" << r->error
//                             << " rsp=" << (r->response ? r->response->toString() : "");
    test2();
}
int main(int args,char ** argv){
    hh::IOManager iom(2);
    iom.schedule(test);
    return 0;
}