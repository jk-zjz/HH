//
// Created by 35148 on 2024/6/14.
//
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <csignal>
#include <iostream>
#include "hook.h"
#include "log.h"
#include "iomanage.h"

static hh::Logger::ptr g_logger = HH_LOG_ROOT();

// 捕获 SIGPIPE 信号的处理函数
void signal_handler(int signal) {
    std::cout << "Caught signal " << signal << std::endl;
}

void test01(){
    hh::IOManager iomanager(1);
    iomanager.schedule([](){
        sleep(2);
        HH_LOG_INFO(g_logger,"1");
    });
    iomanager.schedule([](){
        sleep(7);
        HH_LOG_INFO(g_logger,"2");
    });
}

void test_sock(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        HH_LOG_ERROR(g_logger, "Failed to create socket");
        return;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7070);
    inet_pton(AF_INET, "122.51.206.17", &addr.sin_addr.s_addr);

    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    if(rt) {
        HH_LOG_ERROR(g_logger, "Failed to connect");
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    if(rt <= 0) {
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR)<< "Failed to send data-="<<sock;
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    if(rt <= 0) {
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR)<< "Failed to receive data";
        return;
    }

    buff.resize(rt);
    HH_LOG_INFO(g_logger, buff);

    // 关闭 socket
    close(sock);
}

int main(){
    hh::IOManager iomanager;
    iomanager.schedule(test_sock);

    return 0;
}