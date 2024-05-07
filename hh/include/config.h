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

namespace hh {
    //配置文件基类
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const std::string & name,const std::string & description = "")
        :m_name(name),m_description(description){}
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

    template<class T>
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
                return boost::lexical_cast<std::string>(m_val);
            }catch (std::exception &e){
                HH_LOG_FAT_ERROR(HH_LOG_ROOT(),"ConfigVar::toString exception %s convert: %s to string"
                                 ,e.what(),typeid(m_val).name())
            }
            return "";
        };
        bool fromString(const std::string &var) override{
            try{
                m_val = boost::lexical_cast<T>(var);
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
            if(name.find_first_not_of("qazxswedcvfrtgbnhyujmkiolp._QAZXSWEDCVFRTGBNHYUJMKIOLP0987654321")
                !=std::string::npos){
                HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::ERROR)<<"Lookup Name Invalid "<<name<<" exists";
                throw std::invalid_argument(name);
            }
            //返回新创建号的
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,define_t,description));
            std::cout<<v->getValue()<<std::endl;
//            ss_data[name]=1;
            s_data[name] = v;
            std::cout<<v->getValue()<<std::endl;
            return v;
        }

        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name){
            std::cout<<s_data.size()<<std::endl;
            auto it = s_data.find(name);
            if(it==s_data.end()){
                return nullptr;
            }
           return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
    private:
        static ConfigVarMap s_data;
    };

}
#endif //HH_CONFIG_H
