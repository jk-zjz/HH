//
// Created by 35148 on 2024/7/27.
//
#include "uri.h"
#include <sstream>
#include <iostream>
namespace hh{
    namespace http{

        Address::ptr Uri::createAddress() const {
            IPAddress::ptr sharedPtr = Address::lookupAnyIPAddress(m_host);
            if(sharedPtr){
                sharedPtr->setPort(m_port);
            }
            return sharedPtr;
        }

        Uri::ptr Uri::Create(const std::string &url) {
            //    foo://userinfo@host:port/path?query#fragment

            try {
                boost::urls::url u(url);
                std::cout << "Scheme: " << u.scheme() << '\n';
                std::cout << "User info: " << u.userinfo() << '\n';
                std::cout << "Host: " << u.host() << '\n';
                std::cout << "Port: " << u.port() << '\n';
                std::cout << "Path: " << u.path() << '\n';
                std::cout << "Query: " << u.query() << '\n';
                std::cout << "Fragment: " << u.fragment() << '\n';
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
            return nullptr;
        }

        Uri::Uri():
        m_port(0){

        }

        std::string Uri::toString() const {
            std::stringstream os;
            dump(os);
            return os.str();
        }


        bool Uri::isDefaultPort()const {
            if(m_port == 0){
                return true;
            }
            if(m_scheme == "http" ||
                    m_scheme == "ws"){
                return m_port == 80;
            }else if(m_scheme == "https" ||
            m_scheme == "wss"){
                return m_port == 443;
            }
            return false;
        }

        std::ostream & Uri::dump(std::ostream& os) const{
            os<<m_scheme<<"://"
            <<(m_userinfo.empty()?"":m_userinfo+"@")
            <<m_host
            <<(isDefaultPort()?"":m_host+":"+std::to_string(m_port))
            <<m_path
            <<(m_query.empty()?"":"?"+m_query)
            <<(m_fragment.empty()?"":"#"+m_fragment);
            return os;
        }
    }
}