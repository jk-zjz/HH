//
// Created by 35148 on 2024/7/15.
//
#include "tcp_server.h"
#include "log.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
void test(){
    auto addr =hh::Address::lookupAny("0.0.0.0:33023");
    auto addr2 = hh::UnixAddress::ptr(new hh::UnixAddress("/tmp/bbll_"));
    std::vector<hh::Address::ptr>addrs;
    addrs.emplace_back(addr);
    addrs.emplace_back(addr2);
    hh::TcpServer::ptr tcp(new hh::TcpServer);
    std::vector<hh::Address::ptr>fails;
    while(!tcp->bind(addrs,fails)){
        sleep(2);
    }
    tcp->start();
}
int main(int args , char **argv){
    hh::IOManager iom(2);
    iom.schedule(test);
    return 0;
}