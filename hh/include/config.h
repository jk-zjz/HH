//
// Created by 35148 on 2024/5/5.
//
#ifndef HH_CONFIG_H
#define HH_CONFIG_H

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "log.h"

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

        }

        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name){
            auto it = s_data.find(name);
            if(it==s_data.end()){
                return nullptr;
            }
            return it;
        }
    private:
        static ConfigVarMap s_data;
    };

}
#endif //HH_CONFIG_H
