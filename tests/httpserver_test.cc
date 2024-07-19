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
    http_server->start();
}
int main(int argc,char ** argv){
    hh::IOManager iom(2);
    iom.schedule(run);
}