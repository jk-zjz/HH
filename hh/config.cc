//
// Created by 35148 on 2024/5/6.
//
#include "config.h"
//"A.B",10
//A:
//  B:10
//  C:20
//"A.C",20;
namespace hh {
    Config::ConfigVarMap Config::s_data;
    static void ListAllMember(const std::string &prefix,
                              const YAML::Node &root,
                              std::list<std::pair<std::string, const YAML::Node>> &all_node) {
        if (prefix.find_first_not_of("qazxswedcvfrtgbnhyujmkiolpQAZXSWEDCVFRTGBNHYUJMKIOLP._0987654321")
            != std::string::npos) {
            //不合法
            HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::ERROR) << "Config invalid name :" << prefix << " :" << root;
            return;
        }
        all_node.emplace_back(prefix, root);
        if (root.IsMap()) {
            for (auto it = root.begin();
                 it != root.end(); it++) {
                ListAllMember(prefix.empty() ? it->first.as<std::string>() : prefix + "." + it->first.as<std::string>(),
                              it->second, all_node);
            }
        }
    }

    void Config::loadFromYaml(const YAML::Node &root) {
        std::list<std::pair<std::string, const YAML::Node>> all_node;
        ListAllMember("", root, all_node);
        for (auto &i: all_node) {
            std::string key=i.first;
            if(key.empty()){
                continue;
            }
            std::transform(key.begin(),key.end(),key.begin(),::tolower);
            ConfigVarBase::ptr  var = LookupBase(key);
            if(var){
                if(i.second.IsScalar()){
                    var->fromString(i.second.Scalar());
                }else{
                    std::stringstream ss;
                    ss<<i.second;
                    var->fromString(ss.str());
                }
            }
        }
    }

    ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
        auto it = s_data.find(name);
        if (it == s_data.end()) {
            return nullptr;
        } else {
            return it->second;
        }
    }
}