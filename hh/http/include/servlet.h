//
// Created by 35148 on 2024/7/21.
//

#ifndef HH_SERVLET_H
#define HH_SERVLET_H
#include <memory>
#include <string>
#include "http.h"
#include "http_session.h"
#include <functional>
#include <unordered_map>
#include "thread.h"
namespace hh{
    namespace http{
        class Servlet{
        public:
            typedef std::shared_ptr<Servlet> ptr;
            Servlet(std::string name)
                :m_name(name){}

            virtual ~Servlet(){};
            // 处理请求
            virtual int32_t handle(hh::http::HttpRequest::ptr request,
                                   hh::http::HttpResponse::ptr response,
                                   hh::http::HttpSession::ptr session);

            std::string getName()const{return m_name;}
        private:
            std::string m_name;
        };
        class FunctionServlet : public Servlet{
        public:
            typedef std::shared_ptr<FunctionServlet> ptr;
            typedef std::function<int32_t(hh::http::HttpRequest::ptr request,
                                          hh::http::HttpResponse::ptr response,
                                          hh::http::HttpSession::ptr session)> callback;
            FunctionServlet(callback cb);
            // 处理请求
            int32_t handle(hh::http::HttpRequest::ptr request,
                            hh::http::HttpResponse::ptr response,
                            hh::http::HttpSession::ptr session) override;
        private:
            callback m_cb;
        };

        // 特殊servlet
        class ServletDispatch : public Servlet{
        public:
            typedef std::shared_ptr<ServletDispatch> ptr;

            typedef RWMutex RWMutexType;
            ServletDispatch();
            int32_t handle(hh::http::HttpRequest::ptr request,
                                   hh::http::HttpResponse::ptr response,
                                   hh::http::HttpSession::ptr session) override;

            void addServlet(const std::string& uri,Servlet::ptr servlet);
            void addServlet(const std::string& uri,FunctionServlet::callback cb);
            void addGlobServlet(const std::string& uri,Servlet::ptr servlet);
            void addGlobServlet(const std::string& uri,FunctionServlet::callback cb);

            void delServlet(const std::string& uri);
            void delGlobServlet(const std::string& uri);

            Servlet::ptr getdefault()const {return m_default;}
            void setdefault(Servlet::ptr v){m_default = v;}

            Servlet::ptr getServlet(const std::string& uri);
            Servlet::ptr getGlobServlet(const std::string& uri);

            Servlet::ptr getMatchServlet(const std::string& uri);
        private:
            // uri ( hh/xxx) ---> servlet  是指定，准确查找
            std::unordered_map<std::string,Servlet::ptr> m_datas;
            // uri (hh/*) ----> servlet 模糊查找
            std::vector<std::pair<std::string,Servlet::ptr>> m_globals;
            // 默认的servlet
            Servlet::ptr m_default;
            // 锁
            RWMutexType m_mutex;
        };
    }
}


#endif //HH_SERVLET_H
