//
// Created by 35148 on 2024/7/11.
//
#include "http.h"
#include <iostream>
void test_request(){
    hh::http::HttpRequest req;
    req.setMethod(hh::http::HttpMethod::GET);
    req.setVersion(0x11);
    req.setHeader("Connection","keep-alive");
    req.setHeader("Accept","*/*");
    req.setHeader("User-Agent","curl/7.64.1");
    req.setHeader("Host","127.0.0.1:8080");
    req.setHeader("Accept-Encoding","gzip, deflate");

    req.setBody("hello world");
    req.dump(std::cout)<<std::endl;
}
void test_response(){
    hh::http::HttpResponse::ptr rsp(new hh::http::HttpResponse);
    rsp->setStatus(hh::http::HttpStatus::LOOP_DETECTED);
    rsp->setHeader("Connection","keep-alive");
    rsp->setHeader("content-type","text/plain");

    rsp->setBody("hello world");
    rsp->dump(std::cout)<<std::endl;

}
int main(int args, char **argv){
    test_request();
    test_response();
    return 0;
}