//
// Created by 35148 on 2024/7/2.
//
#include "socket.h"
#include "log.h"
#include "iomanage.h"
#include "config.h"
#include <yaml-cpp/yaml.h>
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
static hh::ConfigVar<std::string>::ptr IP;
static hh::ConfigVar<uint16_t>::ptr PORT;
// 客户端
void test_01(){
    // 初始化ip地址与端口
    hh::IPAddress::ptr addr = hh::IPAddress::Create(IP->getValue().c_str(), PORT->getValue());
    if(!addr){
        return;
    }
    // 创建socket TCP 连接
    hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
    // 连接
    if(!sock->connect(addr)){
        return;
    }
    char buff2[4096];
    // 直接获取，因为send 和recv 被hook了，我们在hook中已经把异步变成同步了
    int rt = sock->recv(buff2,sizeof(buff2));
    if(rt <= 0){
        std::cout<<"recv failed"<<std::endl;
    }else{
        std::cout<<buff2<<std::endl;
    }
    // 关闭连接
    sock->close();
}
void test(){
    HH_LOG_INFO(g_logger,IP->getValue());
    HH_LOG_FAT_INFO(g_logger,"%u",PORT->getValue())
    // 创建ip地址
    hh::IPAddress::ptr addr = hh::Address::lookupAnyIPAddress(IP->getValue());
    addr->setPort(PORT->getValue());
    // 创建socket TCP 连接
    hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
    // 绑定并且监听
    sock->bind(addr);
    sock->listen();
    while(true){
        // 提取请求
        hh::Socket::ptr sock2 = sock->accept();
        if(sock2){
            sock2->send("hello world\n", strlen("hello world\n"));
            // 获取本地信息
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<sock2->getLocalAddress()->toString();
            // 获取远端信息
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<sock2->getRemoteAddress()->toString();
        }else{
            std::cout<<"accept failed"<<std::endl;
            break;
        }
    }
}
int main(){
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/ip.yml");

    // 先约定   因为约定>配置
    IP =hh::Config::Lookup("IP.ip",(std::string)"0.0.0.0","");
    PORT =hh::Config::Lookup("IP.port",(uint16_t)0,"");
    // 在加载配置文件
    hh::Config::loadFromYaml(root);

    HH_LOG_INFO(g_logger,IP->getValue());
    // 创建IOManager
    hh::IOManager iom;
    // 加入任务
    iom.schedule(test);
    // 同步延时秒
    sleep(2);
    // 添加定时器任务，并且循环执行
    iom.addTimer(1000,test_01, true);
    return 0;
}