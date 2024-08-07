//
// Created by 35148 on 2024/8/5.
//
#include "iomanage.h"
#include "log.h"
#include "http_server.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
void run(){
    g_logger->setLevel(hh::LogLevel::DEBUG);
    hh::http::HttpServer::ptr server(new hh::http::HttpServer(true));
    hh::Address::ptr addr = hh::Address::lookupAnyIPAddress("0.0.0.0:7080");
    if(!addr){
        HH_LOG_ERROR(g_logger,"get address fail");
        return;
    }
    while(!server->bind(addr)){
        sleep(2);
        HH_LOG_ERROR(g_logger,"bind fail");
    }
    server->start();
}
int main(int argc, char *argv[]){
    hh::IOManager iomanage(1);
    iomanage.schedule(run);

    return 0;
}