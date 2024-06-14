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
#include <fcntl.h>


void test_fiber() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "110.242.68.66", &addr.sin_addr.s_addr);
    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "add event errno=" << errno << " " << strerror(errno);
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::READ, [](){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "read callback";
        });
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::WRITE, [&sock](){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "write callback";
            //close(sock);
            hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::READ);
        });
    } else {
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "else " << errno << " " << strerror(errno);
    }

}
void test01(){
    hh::IOManager iom(2, false,"JK");
    iom.schedule(&test_fiber);
}
hh::Timer::ptr timer;
hh::Timer::ptr timer1;
void test02(){
    hh::IOManager iom(1);
    timer = iom.addTimer(1000, [](){
        static int count = 0;
        HH_LOG_INFO(g_logger, "hello");
        if(++count == 4) {
            timer->reset(500, true);
        }
        if(count == 7){
            timer->cancel();
        }
    }, true);
    timer1 = iom.addTimer(2000, [](){
        static int counts = 0;
        HH_LOG_INFO(g_logger, "---hello---");
        if(++counts == 4) {
            timer1->reset(500, true);
        }
        if(counts == 7){
            timer1->cancel();
        }
    }, true);
}
int main(){
    //test01();
    test02();
}