//
// Created by 35148 on 2024/7/17.
//
#include "tcp_server.h"
#include "log.h"
#include "bytearray.h"
#include "http_parser.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
class EchoServer:public hh::TcpServer{
public:
    EchoServer(int type):m_type(type){};
    void handleClient(hh::Socket::ptr client)override;
private:
    int m_type = 1; // 1 文本输出 2 二进制输出
};
void EchoServer::handleClient(hh::Socket::ptr client) {
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "handleClient"<<*client;
    hh::ByteArray::ptr ba(new hh::ByteArray);
    while (true){
        // 清空
        ba->clear();
        std::vector<iovec>iovec;
        // 获取可写缓冲区
        ba->getWriteBuffers(iovec,1024);
        // 读取数据
        int rt = client->recv(&iovec[0],iovec.size());
        if(rt == 0){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "client close";
            break;
        }else if(rt < 0){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "client error";
            break;
        }
        ba->setPosition(ba->getPosition()+rt);
        ba->setPosition(0);
        if(m_type == 1){
//            std::cout<<ba->toString();
            // http解析
            hh::http::HttpRequestParser::ptr parser(new hh::http::HttpRequestParser);
            parser->execute((char * )ba->toString().c_str(),ba->toString().size());
            std::cout<<parser->getData()->toString();
        }else{
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "client data:"<<ba->toHexString();
        }
    }
}
int type = 1;
void run(){
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<"server type="<<type;
    EchoServer::ptr es(new EchoServer(type));
    auto address = hh::Address::lookupAny("0.0.0.0:12345");
    while (!es->bind(address)){
        sleep(2);
    }
    es->start();
}
int main(int args,char ** argv){
    if(args<2){
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "usage: "<<argv[0]<<"-t type";
        return 0;
    }
    std::cout<<"type:"<<argv[1]<<std::endl;
    if(!strncmp(argv[1],"-b",2)){
        type = 2;
    }
    hh::IOManager iomanage(2);
    iomanage.schedule(run);
    return 0;
}