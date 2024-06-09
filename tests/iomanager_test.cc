//
// Created by 35148 on 2024/6/9.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "iomanage.h"
#include "log.h"
hh::Logger::ptr g_logger = HH_LOG_ROOT();
void test_fiber(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("39.156.66.10");
    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))){
    }else if(errno == EINPROGRESS){
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::READ, [](){
            HH_LOG_INFO(g_logger,"read callback");
        });
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::WRITE, [](){
            HH_LOG_INFO(g_logger,"write callback");
        });

    }
}
void test01(){
    hh::IOManager iom(1, false,"JK");
    iom.schedule(&test_fiber);
}
int main(){
    test01();
}