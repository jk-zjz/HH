//
// Created by 35148 on 2024/5/5.
//
#ifndef HH_CONFIG_H
#define HH_CONFIG_H

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "util.h"
#include <yaml-cpp/yaml.h>
#include <list>
#include <utility>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include "thread.h"
#include "log.h"
namespace hh {
    class Config;
    //配置文件基类
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const std::string & name,const std::string & description = "")
        :m_name(name),m_description(description){
            std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
        }
        ~ConfigVarBase(){};
        const std::string& getName(){return m_name;}
        const std::string& getDescription(){return m_description;}

        //转字符串
        virtual std::string  toString() = 0;
        //字符串转指定类
        virtual bool fromString(const std::string &var) = 0;
        virtual std::string gettype() =0;
    protected:
        std::string m_name;         //名称
        std::string m_description;  //描述
    };
    //转换  F--->T
    template<class F,class T>
    class LexicalCast{
    public:
        T operator()(const F& v){
            return boost::lexical_cast<T>(v);
        }
    };
    //特化vector模板
    template<class T>
    class LexicalCast<std::string,std::vector<T>> {
    public:
        std::vector<T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::vector<T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss<<i;
                vce.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::vector<T>,std::string> {
    public:
        std::string operator()(const std::vector<T>& var){
            YAML::Node node;
            std::stringstream ss;
            for(auto &i:var){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss<<node;
            return ss.str();
        }
    };
    //特化list类
    template<class T>
    class LexicalCast<std::string,std::list<T>> {
    public:
        std::list<T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::list<T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss<<i;
                vce.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::list<T>,std::string> {
    public:
        std::string operator()(const std::list<T>& var){
            YAML::Node node;
            std::stringstream ss;
            for(auto &i:var){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss<<node;
            return ss.str();
        }
    };
    //转换set类
    template<class T>
    class LexicalCast<std::string,std::set<T>> {
    public:
        std::set<T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::set<T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss<<i;
                vce.insert(LexicalCast<std::string,T>()(ss.str()));
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::set<T>,std::string> {
    public:
        std::string operator()(const std::set<T> &var) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &i: var) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            ss << node;
            return ss.str();
        }
    };
    //转换unordered_set类
    template<class T>
    class LexicalCast<std::string,std::unordered_set<T>> {
    public:
        std::unordered_set<T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::unordered_set<T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss<<i;
                vce.insert(LexicalCast<std::string,T>()(ss.str()));
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::unordered_set<T>,std::string> {
    public:
        std::string operator()(const std::unordered_set<T> &var) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &i: var) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            ss << node;
            return ss.str();
        }
    };
    //转换map类
    template<class T>
    class LexicalCast<std::string,std::map<std::string,T>> {
    public:
        std::map<std::string,T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::map<std::string,T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss.str("");
                ss<<i.second;
                vce.insert({i.first.Scalar(),LexicalCast<std::string,T>()(ss.str())});
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::map<std::string,T>,std::string> {
    public:
        std::string operator()(const std::map<std::string,T> &var) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &i: var) {
                node[i.first]=YAML::Load(LexicalCast<T,std::string>()(i.second));
            }
            ss << node;
            return ss.str();
        }
    };
    //转换 unordered_map累
    template<class T>
    class LexicalCast<std::string,std::unordered_map<std::string,T>> {
    public:
        std::unordered_map<std::string,T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::unordered_map<std::string,T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss.str("");
                ss<<i.second;
                vce.insert({i.first.Scalar(),LexicalCast<std::string,T>()(ss.str())});
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::unordered_map<std::string,T>,std::string> {
    public:
        std::string operator()(const std::unordered_map<std::string,T> &var) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &i: var) {
                node[i.first]=YAML::Load(LexicalCast<T,std::string>()(i.second));
            }
            ss << node;
            return ss.str();
        }
    };
    //FromStr T operator()(const std::string&)
        //吧string转为我需要的类型
    //ToStr std::string operator()(const T&)
        //吧我的类型转换为string
    //使用的都是仿函数
    template<class T,class FromStr=LexicalCast<std::string,T>
                    ,class ToStr=LexicalCast<T,std::string>>
    class ConfigVar:public ConfigVarBase{
    public:
        typedef std::shared_ptr<ConfigVar> ptr;
        typedef RWMutex RWMutexType;
        typedef std::function<void (const T& old_value,const T& new_value)> on_change_cb;
        ConfigVar(const std::string& name
                  ,const T& define_t
                  ,const std::string & description = ""):
                  ConfigVarBase(name,description),m_val(define_t){
        }
        std::string toString() override{
            try {
                RWMutexType::ReadLock lock(m_mutex);
                return ToStr()(m_val);
                //return boost::lexical_cast<std::string>(m_val);
            }catch (std::exception &e){
                HH_LOG_FAT_ERROR(HH_LOG_ROOT(),"ConfigVar::toString exception %s convert: %s to string"
                                 ,e.what(),typeid(m_val).name())
            }
            return "";
        };
        bool fromString(const std::string &var) override{
            try{
                setValue(FromStr()(var));
                //m_val = boost::lexical_cast<T>(var);
            }catch (std::exception&e){
                HH_LOG_FAT_ERROR(HH_LOG_ROOT(),"ConfigVar::fromString exception %s convert: %s to string /n [%s]"
                ,e.what(),typeid(m_val).name(),var.c_str())
            }
            return false;
        };
        const T getValue(){
            RWMutexType::ReadLock lock(m_mutex);
            return m_val;
        }
        //值发生改变产生回调
        void setValue(const T &t){
            {
                RWMutexType::ReadLock lock(m_mutex);
                //判断是否改变值，可让回调函数触发
                if(t==m_val){
                    return;
                }
                for(auto &i:m_ocb){
                    i.second(m_val,t);
                }
            }
            RWMutexType::WriteLock lock(m_mutex);
            m_val=t;
        }
        std::string gettype() override{return typeid(T).name();};

        //回调函数 新增，删除，清空与查找
        on_change_cb getOcb(uint64_t key){
            RWMutexType::ReadLock lock(m_mutex);
            auto it = m_ocb.find(key);
            return it==m_ocb.end()? nullptr : it->second;
        }
        uint64_t addOcb(on_change_cb ocb){
            static uint64_t s_key=0;
            RWMutexType::WriteLock lock(m_mutex);
            s_key++;
            if(!m_ocb.count(s_key)){
                s_key++;
            }
            m_ocb[s_key]=ocb;
            return s_key;
        }
        void deleteOcb(uint64_t key){
            RWMutexType::WriteLock lock(m_mutex);
            if(!m_ocb.count(key)){
                HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<"delete not exist key";
                return;
            }
            m_ocb.erase(key);
        }
        void eraseOcb(){
            RWMutexType::WriteLock lock(m_mutex);
            m_ocb.erase();
        }
        std::map<uint64_t,on_change_cb> getOcb()const {return m_ocb;}
    private:
        T m_val;
        std::map<uint64_t,on_change_cb> m_ocb;
        RWMutexType m_mutex;
    };
    /**
     * @config 配置类
     *
     * */
    class Config{
    public:
        typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;
        typedef RWMutex RWMutexType;
        template<class T>
        static typename ConfigVar<T>::ptr Lookup(
                const std::string& name
                ,const T& define_t
                ,const std::string & description = ""){
            std::string m_name(name);
            {
                RWMutexType::ReadLock lock(getRWMutex());
                //判断name合不合法
                std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
                auto it = getData().find(m_name);
                if (it != getData().end()) {
                    auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                    if (!tmp) {
                        //转换失败
                        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::ERROR) << "Lookup Name = "
                                                                               << name << " exists type no "
                                                                               << it->second->gettype() << " "
                                                                               << it->second->toString();
                        return tmp;
                    }
                    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::INFO) << "Lookup Name = " << name << " exists";
                    return tmp;
                }
                if (m_name.find_first_not_of("qazxswedcvfrtgbnhyujmkiolp[]._0987654321")
                    != std::string::npos) {
                    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::ERROR) << "Lookup Name Invalid " << name
                                                                           << " exists";
                    throw std::invalid_argument(m_name);
                }
            }
            RWMutexType::WriteLock lock(getRWMutex());
            //返回新创建号的
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(m_name,define_t,description));
            getData()[m_name] = v;
            return v;
        }

        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name){
            RWMutexType::ReadLock lock(getRWMutex());
            auto it = getData().find(name);
            if(it==getData().end()){
                return nullptr;
            }
           return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
        //静态配置解析类
        static void loadFromYaml(const YAML::Node& root);
        //判断是否存在
        static ConfigVarBase::ptr LookupBase(const std::string& name);
        //自定义打印函数
        static void visit(std::function<void(ConfigVarBase::ptr)> cb);
    private:
        static ConfigVarMap & getData(){
            static ConfigVarMap s_data;
            return s_data;
        }
        static  RWMutexType& getRWMutex(){
            static RWMutexType s_mutex;
            return s_mutex;
        }
    };

}
#endif //HH_CONFIG_H
