//
// Created by 35148 on 2024/7/2.
//
#include "socket.h"
#include "log.h"
#include "iomanage.h"
#include <string>
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
void test_01(){
    hh::IPAddress::ptr addr = hh::Address::lookupAnyIPAddress("www.baidu.com");
    if(addr){
        std::cout<<addr->toString()<<std::endl;
    }else{
        std::cout<<"error"<<std::endl;
    }
    hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
    addr->setPort(80);
    if(!sock->connect(addr)){
        std::cout<<"connect "<<addr->toString()<<" failed"<<std::endl;
    }else{
        std::cout<<"connect "<<addr->toString()<<" connected"<<std::endl;
    }
    std::string buff = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff.c_str(),buff.size());
    if(rt <= 0){
        std::cout<<"send failed"<<std::endl;
    }else{
        std::cout<<"send "<<rt<<" bytes"<<std::endl;
    }
    char buff2[4096];
    rt = sock->recv(buff2,sizeof(buff2));
    if(rt <= 0){
        std::cout<<"recv failed"<<std::endl;
    }else{
        std::cout<<"recv "<<rt<<" bytes"<<std::endl;
        std::cout<<buff2<<std::endl;
    }
    sock->close();
}
int main(){
    hh::IOManager iom;
    iom.schedule(test_01);
    return 0;
}