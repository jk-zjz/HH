//
// Created by 35148 on 2024/6/26.
//
#include "address.h"
#include "log.h"
static hh::Logger::ptr g_logger = HH_LOG_ROOT();

void test_Address_ip(){
    std::vector<hh::Address::ptr> addrs;
    hh::Address::lookup(addrs,"[2408:874d:a00:9::15]:8080",AF_INET6);
    for(auto& i : addrs){
        HH_LOG_INFO(g_logger,i->toString());
    }
}
void test_Address_net(){
    std::multimap<std::string,std::pair<hh::Address::ptr, uint32_t>>result;
    hh::Address::GetInterfaceAddresses(result);
    for(auto& i : result){
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<i.first << " - " << i.second.first->toString() << " - " << i.second.second;
    }
}
void test_AddressIPv4(){
    auto addr = hh::IPAddress::Create("103.101.204.135",443);
    if(addr){
        HH_LOG_INFO(g_logger,addr->toString());
    }else{
        HH_LOG_ERROR(g_logger,"error");
    }
}
int main(){
    int a = 0x123456;
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<a;
    test_Address_ip();
    test_Address_net();
    test_AddressIPv4();
    hh::IPv4Address::ptr addr = hh::IPv4Address::Create("127.0.0.1",8070);
    HH_LOG_INFO(g_logger,addr->toString());
    auto au = addr->broadcastAddress(24);
    HH_LOG_INFO(g_logger,au->toString());
    auto an = addr->networkAddress(24);
    HH_LOG_INFO(g_logger,an->toString());
    auto af =addr->subnetMask(24);
    HH_LOG_INFO(g_logger,af->toString());
    HH_LOG_FAT_INFO(g_logger,"%d",addr->getPort());
    return 0;
}