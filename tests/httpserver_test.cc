//
// Created by 35148 on 2024/7/19.
//
#include "http_server.h"

void run(){
    hh::http::HttpServer::ptr http_server(new hh::http::HttpServer);
    hh::Address::ptr addr = hh::Address::lookupAnyIPAddress("0.0.0.0:12345");
    while (!http_server->bind(addr)){
        sleep(2);
    }
    auto it = http_server->getServletDispatch();
    it->addServlet("/hello/xx",
                   [](hh::http::HttpRequest::ptr req,
                           hh::http::HttpResponse::ptr rsp,
                           hh::http::HttpSession::ptr session){

                std::cout<<req->toString();
                rsp->setBody("<h1>SB<h1>");
               rsp->setHeader("Content-Type","text/html; charset=utf-8");
               rsp->setHeader("Server","HH/1.0.0");
        return 0;
    });
    it->addGlobServlet("/hello/*",
                   [](hh::http::HttpRequest::ptr req,
                      hh::http::HttpResponse::ptr rsp,
                      hh::http::HttpSession::ptr session){

                       rsp->setBody("<h1>NH<h1>");
                       rsp->setHeader("Content-Type","text/html; charset=utf-8");
                       rsp->setHeader("Server","HH/1.0.0");
                       return 0;
                   });
    http_server->start();
}
int main(int argc,char ** argv){
    hh::IOManager iom(2);
    iom.schedule(run);
}