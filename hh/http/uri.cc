//
// Created by 35148 on 2024/7/27.
//
#include "uri.h"
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
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
            Uri::ptr uri(new Uri);
            try {

                boost::urls::url u(url);
                uri->m_scheme = u.scheme();
                uri->m_host = u.host();
                std::string s_port =u.port().empty() ? "0" : u.port();
                if(s_port == "0"){
                    if(uri->m_scheme =="http" || uri->m_scheme == "ws"){
                        s_port = "80";
                    }else if(uri->m_scheme == "https" || uri->m_scheme == "wss"){
                        s_port = "443";
                    }
                }
                uri->m_port = boost::lexical_cast<std::uint16_t>(s_port);
                uri->m_path = u.path();
                uri->m_query = u.query();
                uri->m_fragment = u.fragment();
                uri->m_userinfo = u.userinfo();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
            return uri;
        }

        Uri::Uri():
        m_port(0){

        }

        std::string Uri::toString()  {
            std::stringstream os;
            dump(os);
            return os.str();
        }


        bool Uri::isDefaultPort() {
            if(m_port != 0){
                return true;
            }
            if(m_scheme == "http" ||
                    m_scheme == "ws"){
                m_port = 80;
            }else if(m_scheme == "https" ||
            m_scheme == "wss"){
                m_port = 443;
            }
            return false;
        }

        std::ostream & Uri::dump(std::ostream& os){
            os<<m_scheme<<"://"
            <<(m_userinfo.empty()?"":m_userinfo+"@")
            <<m_host
            <<(isDefaultPort()?":"+std::to_string(m_port):"")
            <<m_path
            <<(m_query.empty()?"":"?"+m_query)
            <<(m_fragment.empty()?"":"#"+m_fragment);
            return os;
        }
    }
}