//
// Created by 35148 on 2024/7/21.
//
#include "servlet.h"
namespace hh{
    namespace http{

        int32_t Servlet::handle(hh::http::HttpRequest::ptr request, hh::http::HttpResponse::ptr response,
                                hh::http::HttpSession::ptr session) {
            return 0;
        }

        FunctionServlet::FunctionServlet(FunctionServlet::callback cb):
                Servlet("ServletFunction"),
                m_cb(cb){
        }

        int32_t FunctionServlet::handle(hh::http::HttpRequest::ptr request, hh::http::HttpResponse::ptr response,
                                        hh::http::HttpSession::ptr session) {
            return m_cb(request, response, session);
        }

        int32_t ServletDispatch::handle(hh::http::HttpRequest::ptr request, hh::http::HttpResponse::ptr response,
                                        hh::http::HttpSession::ptr session) {
//            return Servlet::handle(request, response, session);
        }
        void ServletDispatch::addServlet(const std::string& uri,Servlet::ptr servlet){
            RWMutexType::WriteLock lock(m_mutex);
            m_datas[uri] = std::move(servlet);
        }
        void ServletDispatch::addServlet(const std::string& uri,FunctionServlet::callback cb){
            RWMutexType::WriteLock lock(m_mutex);
            m_datas[uri].reset(new FunctionServlet(std::move(cb)));
        }
        void ServletDispatch::addGlobServlet(const std::string& uri,Servlet::ptr servlet){
            RWMutexType::WriteLock lock(m_mutex);
            for(auto i = m_globals.begin();
                        i != m_globals.end(); ++i){
                if(i->first == uri){
                    m_globals.erase(i);
                    break;
                }
            }
            m_globals.push_back(std::make_pair(uri,servlet));
        }
        void ServletDispatch::addGlobServlet(const std::string& uri,FunctionServlet::callback cb){
            return addGlobServlet(uri,FunctionServlet::ptr(new FunctionServlet(std::move(cb))));
        }
        void ServletDispatch::delServlet(const std::string& uri){
            RWMutexType::WriteLock lock(m_mutex);
            m_datas.erase(uri);
        }
        void ServletDispatch::delGlobServlet(const std::string& uri){
            for(auto i = m_globals.begin();
                    i != m_globals.end(); ++i){
                if(i->first == uri){
                    m_globals.erase(i);
                    break;
                }
            }
        }
        Servlet::ptr ServletDispatch::getServlet(const std::string& uri){
            RWMutexType::ReadLock lock(m_mutex);
            return m_datas[uri];
        }
        Servlet::ptr ServletDispatch::getGlobServlet(const std::string& uri){

        }
        Servlet::ptr ServletDispatch::getMatchServlet(const std::string& uri){

        }

        ServletDispatch::ServletDispatch():Servlet("ServletDispatch") {

        }
    }
}