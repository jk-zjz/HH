//
// Created by 35148 on 2024/7/21.
//
#include "servlet.h"
#include <fnmatch.h>
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
            Servlet::ptr slt = getMatchServlet(request->getPath());
            if(slt){
                return slt->handle(request, response, session);
            }
            return 0;
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
            auto it = m_datas.find(uri);
            return it == m_datas.end() ? nullptr : it->second;
        }
        Servlet::ptr ServletDispatch::getGlobServlet(const std::string& uri){
            RWMutexType ::ReadLock lock(m_mutex);
            for(auto& i:m_globals){
                if(i.first==uri){
                    return i.second;
                }
            }
            return nullptr;
        }
        Servlet::ptr ServletDispatch::getMatchServlet(const std::string& uri){
            RWMutexType::ReadLock lock(m_mutex);
            auto it = m_datas.find(uri);
            if(it != m_datas.end()){
                return it->second;
            }
            for(auto& i:m_globals){
                if(!fnmatch(i.first.c_str(),uri.c_str(),0)){
                    return i.second;
                }
            }
            return m_default;
        }

        ServletDispatch::ServletDispatch():
        Servlet("ServletDispatch"){
            m_default.reset(new NotFoundServlet());
        }

        NotFoundServlet::NotFoundServlet():
        Servlet("NotFoundServlet") {

        }

        int32_t NotFoundServlet::handle(hh::http::HttpRequest::ptr request, hh::http::HttpResponse::ptr response,
                                        hh::http::HttpSession::ptr session) {
            static std::string body = "<html><head><title>404 Not Found</title>"
                                      "</head><body>"
                                      "<center><h1>HH/1.0.0</h1></center>"
                                      "<center><h1>404 Not Found</h1></center></body></html>";
                response->setHeader("Content-Type","text/html; charset=utf-8");
                response->setHeader("Server","HH/1.0.0");
                response->setStatus(hh::http::HttpStatus::NOT_FOUND);
                response->setBody(body);
                return 0;
        }
    }
}