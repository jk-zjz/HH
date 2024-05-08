//
// Created by 35148 on 2024/5/5.
//
#ifndef HH_CONFIG_H
#define HH_CONFIG_H

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "log.h"
#include "util.h"
#include <yaml-cpp/yaml.h>
#include <list>
#include <utility>
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
        virtual bool fromString(const std::string &var) = 0;
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
        ConfigVar(const std::string& name
                  ,const T& define_t
                  ,const std::string & description = ""):
                  ConfigVarBase(name,description),m_val(define_t){
        }
        std::string toString() override{
            try {
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
                HH_LOG_FAT_ERROR(HH_LOG_ROOT(),"ConfigVar::fromString exception %s convert: %s to string"
                ,e.what(),typeid(m_val).name())
            }
            return false;
        };
        const T getValue()const{return m_val;}
        void setValue(const T &t){m_val=t;}
    private:
        T m_val;
    };
    class Config{
    public:
        typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;
        template<class T>
        static typename ConfigVar<T>::ptr Lookup(
                const std::string& name
                ,const T& define_t
                ,const std::string & description = ""){
//            //判断又没有

            auto  it =Lookup<T>(name);
            if(it){
                HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<"Lookup Name = "<<name<<" exists";
                return it;
            }
            //判断name合不合法
            if(name.find_first_not_of("qazxswedcvfrtgbnhyujmkiolp._0987654321")
                !=std::string::npos){
                HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"Lookup Name Invalid "<<name<<" exists";
                throw std::invalid_argument(name);
            }
            //返回新创建号的
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,define_t,description));
            s_data[name] = v;
            return v;
        }

        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name){
            auto it = s_data.find(name);
            if(it==s_data.end()){
                return nullptr;
            }
           return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
        static void loadFromYaml(const YAML::Node& root);
        static ConfigVarBase::ptr LookupBase(const std::string& name);
    private:
        static ConfigVarMap s_data;
    };

}
#endif //HH_CONFIG_H
