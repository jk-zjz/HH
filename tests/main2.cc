//
// Created by 35148 on 2024/5/6.
//
#include "config.h"
#include <yaml-cpp/yaml.h>
void printNode(const YAML::Node& node) {
    switch (node.Type()) {
        case YAML::NodeType::Null:
            std::cout << "null" << std::endl;
            break;
        case YAML::NodeType::Scalar:
            std::cout <<"  " <<node.as<std::string>() << std::endl;
            break;
        case YAML::NodeType::Sequence:
            for (size_t i = 0; i < node.size(); ++i) {

                printNode(node[i]);
            }
            break;
        case YAML::NodeType::Map:
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
                std::cout <<"key "<< it->first.as<std::string>() << "\n";
                printNode(it->second);
            }
            break;
        default:
            std::cout << "unknown type" << std::endl;
            break;
    }
}
void yaml(){
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
    printNode(root);
   // HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<root;
}
void Confgi(){
    hh::ConfigVar<int>::ptr g_int_value_config_ = hh::Config::Lookup("hh.values",(int)9876,"hh");
    hh::ConfigVar<std::string>::ptr g_in = hh::Config::Lookup("hh",(std::string)"","");
    hh::ConfigVar<std::vector<int>>::ptr get_vector_int = hh::Config::Lookup<std::vector<int>>("hh.arr",{1,2,3},"sb");
    auto vec = get_vector_int->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }

    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_int_value_config_->toString();
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
    hh::Config::loadFromYaml(root);


    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_int_value_config_->toString();
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_in->getValue();
    vec = get_vector_int->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    printf("%s",g_int_value_config_->toString().c_str());
}
void test(){
    hh::ConfigVar<std::string>::ptr str = hh::Config::Lookup<std::string>("Logs[1].formatter","asd","asda");
    std::cout<<str->getValue()<<std::endl;
    hh::ConfigVar<std::list<int>>::ptr  int_list = hh::Config::Lookup<std::list<int>>("hh.list_arr",{1,2,3},"");
    hh::ConfigVar<std::set<int>>::ptr int_set = hh::Config::Lookup<std::set<int>>("hh.set_arr",{2,2,3},"");
    hh::ConfigVar<std::map<std::string,std::string>>::ptr string_map = hh::Config::Lookup("hh",std::map<std::string,std::string>{{"key","2"}},"");
    std::list<int>vec = int_list->getValue();
    std::set<int>m_set= int_set->getValue();
    std::map<std::string,std::string>m_map =string_map->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_set){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_map){
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<i.first<<"  "<<i.second;
    }
    std::cout<<int_list->toString()<<std::endl;
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
    hh::Config::loadFromYaml(root);
    vec = int_list->getValue();
    m_set= int_set->getValue();
    m_map =string_map->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_set){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_map){
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<i.first<<"  "<<i.second;
    }
    std::cout<<int_list->toString()<<std::endl;
    std::cout<<str->getValue();
    std::cout<<int_set->toString()<<std::endl;
}
int main(){
    //Confgi();
    test();
    return 0;
}