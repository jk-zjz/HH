//
// Created by 35148 on 2024/5/6.
//
#include "config.h"
int main(){
    hh::Config c;
    hh::ConfigVar<int>::ptr g_int_value_config = c.Lookup("hh",(int)9876,"hh");
//    hh::ConfigVar<int>::ptr g_int_value_config = hh::Config::Lookup("hh",(int)9876,"hh");
//    if(g_int_value_config== nullptr){
//        std::cout<<"123"<<std::endl;
//    }
    HH_LOG_FAT_INFO(HH_LOG_ROOT(),"%d",g_int_value_config->getValue());
//    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_int_value_config->getValue();
//    std::cout<<"asdas"<<std::endl;
    hh::Config D;
    hh::ConfigVar<int>::ptr g_int_value_config_ =D.Lookup<int>("hh");
    HH_LOG_FAT_INFO(HH_LOG_ROOT(),"%d",g_int_value_config_->getValue());

    return 0;
}