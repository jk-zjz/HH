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
    /**
     * @config 递归遍历解析
     *
     * */
    static void ListAllMember(const std::string &prefix,
                              const YAML::Node &root,
                              std::list<std::pair<std::string, const YAML::Node>> &all_node) {
        std::string m_prefix(prefix);
        std::transform(m_prefix.begin(),m_prefix.end(),m_prefix.begin(),::tolower);
        if (m_prefix.find_first_not_of("qazxswedcvfrtgbnhyujmkiolp.[]_0987654321")
            != std::string::npos) {
            //不合法
            HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::ERROR) << "Config invalid name :" << m_prefix << " :" << root;
            return;
        }
        all_node.emplace_back(m_prefix, root);
        if (root.IsMap()) {
            for (auto it = root.begin();
                 it != root.end(); it++) {
                ListAllMember(m_prefix.empty() ? it->first.as<std::string>() : m_prefix + "." + it->first.as<std::string>(),
                              it->second, all_node);
            }
        }else if(root.IsSequence()){//-----------------------------------------------------------------
            for(size_t i=0;i<root.size();i++){
                std::stringstream  ss;
                ss<<"["<<boost::lexical_cast<std::string>(i)<<"]";
                ListAllMember(m_prefix.empty() ? ss.str() : m_prefix +ss.str(),root[i],all_node);
            }
        }
    }

    /**
     * @config 加载yaml文件
     * */
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
    /**
     * @config 获取配置项是否存在
     * */
    ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
        RWMutexType::ReadLock lock(getRWMutex());
        auto it = getData().find(name);
        if (it == getData().end()) {
            return nullptr;
        } else {
            return it->second;
        }
    }
    /**
     * @config 自定义打印函数
     *
     * */
    void Config::visit(std::function<void(ConfigVarBase::ptr)> cb) {
        RWMutexType::ReadLock lock(getRWMutex());
        ConfigVarMap& m =getData();
        for(auto i=m.begin();
            i!=m.end();i++){
            cb(i->second);
        }
    }
}