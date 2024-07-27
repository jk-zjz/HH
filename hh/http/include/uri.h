//
// Created by 35148 on 2024/7/27.
//

#ifndef HH_URL_H
#define HH_URL_H
#include <memory>
#include "address.h"
#include <fstream>
#include <string>
#include <boost/url.hpp>
namespace hh {
    namespace http{
        class Uri {
            /*
             *      foo://userinfo@host:port/path?query#fragment
             *
             *          std::string m_scheme;       // 协议
             *          std::string m_userinfo;     // 用户信息
             *          uint16_t m_port;            // 端口
             *          std::string m_host;         // 主机
             *          std::string m_path;         // 路径
             *          std::string m_query;        // 查询
             *          std::string m_fragment;     // 片段
             */
        public:
            typedef std::shared_ptr<Uri> ptr;

            const std::string getScheme() const { return m_scheme; }
            const std::string getUserinfo() const { return m_userinfo; }
            const std::string getHost() const { return m_host; }
            uint16_t getPort() const { return m_port; }
            const std::string getPath() const { return m_path; }
            const std::string getQuery() const { return m_query; }
            const std::string getFragment() const { return m_fragment; }

            void setScheme(const std::string& v) { m_scheme = v; }
            void setUserinfo(const std::string& v) { m_userinfo = v; }
            void setHost(const std::string& v) { m_host = v; }
            void setPort(uint16_t v) { m_port = v; }
            void setPath(const std::string& v) { m_path = v; }
            void setQuery(const std::string& v) { m_query = v; }
            void setFragment(const std::string& v) { m_fragment = v; }

            Address::ptr createAddress() const;
            static Uri::ptr Create(const std::string& url);
            Uri();
            std::string toString() const;
            std::ostream & dump(std::ostream& os) const;
        private:
             bool isDefaultPort() const;
        private:
            std::string m_scheme;       // 协议
            std::string m_userinfo;     // 用户信息
            uint16_t m_port;            // 端口
            std::string m_host;         // 主机
            std::string m_path;         // 路径
            std::string m_query;        // 查询
            std::string m_fragment;     // 片段
        };


    }
}


#endif //HH_URL_H
