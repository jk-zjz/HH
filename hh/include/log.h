//
// Created by 35148 on 2024/4/17.
//
#ifndef HH_LOG_H
#define HH_LOG_H

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <memory>
#include <map>
#include <functional>
#include <iomanip>
#include <cstdarg>
#include "singleton.h"
#define HH_LOG_LEVEL(logger,level,Str)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0)))).setSS(Str)

#define HH_LOG_DEBUG(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::DEBUG,Str);
#define HH_LOG_INFO(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::INFO,Str);
#define HH_LOG_WARN(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::WARN,Str);
#define HH_LOG_ERROR(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::ERROR,Str);
#define HH_LOG_FATAL(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::FATAL,Str);


#define HH_LOG_LEVEL_CHAIN(logger,level)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0)))).getEvent()->getSS()

#define HH_LOG_FAT_LEVEL(logger,level,fmt,...)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0)))).getEvent()->format(fmt,__VA_ARGS__)

#define HH_LOG_FAT_DEBUG(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::DEBUG,fmt,__VA_ARGS__);
#define HH_LOG_FAT_INFO(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::INFO,fmt,__VA_ARGS__);
#define HH_LOG_FAT_WARN(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::WARN,fmt,__VA_ARGS__);
#define HH_LOG_FAT_ERROR(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::ERROR,fmt,__VA_ARGS__);
#define HH_LOG_FAT_FATAL(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::FATAL,fmt,__VA_ARGS__);

#define HH_LOG_ROOT() hh::LoggerMgr::GetInstance()->GetRoot()

//声明命名空间，以免名称重复，可 :: 访问
namespace hh {
    class Logger;
    //日志级别
    class LogLevel {
    public:
        enum Level {
            UNKNOW = 0,//异常
            DEBUG = 1,//调试信息
            INFO = 2, //一般信息
            WARN = 3, //不会影响查询正常运行信息，警告
            ERROR = 4,//错误
            FATAL = 5 //严重错误
        };
        static const char *ToString(LogLevel::Level level);
    };
    // 日志事件
    class LogEvent {
    public:
        //定义智能指针类型，别名
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger,
                 LogLevel::Level level,
                 const char *mFile,
                 uint32_t mLine,
                 uint32_t mElapse,
                 uint32_t mThreadId,
                 uint32_t mFiberId,
                 uint64_t mTime);
        //get方法
        const char *getFile() const { return m_file; }

        uint32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        uint32_t getFiberId() const { return m_fiberId; }

        std::string getContent() const { return m_ss.str(); }

        uint64_t getTime() const { return m_time; }

        std::stringstream &getSS() { return m_ss; }

        std::shared_ptr<Logger> getLogger()const {return m_logger;}
        LogLevel::Level getLevel()const {return m_level;}

        void format(const char * fat,...);
        void format(const char * fat,va_list al);
    private:
        const char *m_file = nullptr;  //文件名
        uint32_t m_line = 0;            //行号
        uint32_t m_elapse = 0;          //启动-->时间
        uint32_t m_threadId = 0;        //线程id
        uint32_t m_fiberId = 0;         //协程id
        uint64_t m_time = 0;              //时间戳
        std::stringstream m_ss;          //内容

        std::shared_ptr<Logger> m_logger; //获取日志器
        LogLevel::Level m_level; //日志级别
    };
    //日志事件包装类
    class LogEventWrap{
    public:
        LogEventWrap(LogEvent::ptr logEvent);
        ~LogEventWrap();
        void setSS(const std::string& S);
        std::stringstream & getSS(){return m_logEvent->getSS();};
        LogEvent::ptr& getEvent(){return m_logEvent;}
    private:
        LogEvent::ptr m_logEvent;
    };

    //日志格式器
    class LogFormotter {
    public:
        typedef std::shared_ptr<LogFormotter> ptr;

        LogFormotter(const std::string &pattern);

        //格式化 event 返回
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        std::string get_pattern() const { return m_pattern; }

    public:
        //通过格式，输出
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            virtual ~FormatItem() {};

            virtual void format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        void init();

    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
    };

    //日志输入地(多个输出地-继承)
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;

        void set_level(LogLevel::Level val) { m_level = val; }
        LogLevel::Level get_level()const {return m_level;}
        //多个输出地，需要虚析构，子类可释放
        virtual ~LogAppender() {};

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        //设置日志选器
        void setFormotter(LogFormotter::ptr val) { m_formotter = std::move(val); }

        LogFormotter::ptr getFormotter() { return m_formotter; }

    protected:
        LogFormotter::ptr m_formotter;    //日志格式器选择
        LogLevel::Level m_level = LogLevel::UNKNOW;        //日志级别
    };

    //日志器
    class Logger : public std::enable_shared_from_this<Logger> {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string &name = "root");

        void Log(LogLevel::Level level, LogEvent::ptr event);

        //日志输出函数
        void debug(LogEvent::ptr event);

        void info(LogEvent::ptr event);

        void warn(LogEvent::ptr event);

        void error(LogEvent::ptr event);

        void fatal(LogEvent::ptr event);

        //添加输出路径
        void addAppender(LogAppender::ptr appender);

        void delectAppender(LogAppender::ptr appender);

        //设置(获取)什么日志记录
        LogLevel::Level getLevel() const { return m_level; }

        void setLevel(LogLevel::Level level) { m_level = level; }

        const std::string &getName() const { return m_name; }

        LogFormotter::ptr get_formotter() const { return m_formotter; }

        void set_formotter(LogFormotter::ptr ptr1) {
            m_formotter=std::move(ptr1); }

    private:
        std::string m_name;                         //谁初始化日志
        LogLevel::Level m_level;                    //日志级别
        std::list<LogAppender::ptr> m_appenders;    //日志输出地集合
        LogFormotter::ptr m_formotter;              //日志格式器
    };

    //输出控制台LogAppender
    class StdoutLogAppender : public LogAppender {
    public:
        std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    class FileLogAppender : public LogAppender {
    public:
        std::shared_ptr<FileLogAppender> ptr;

        FileLogAppender(const std::string &filename);

        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

        //文件的重新读取-true为打开
        bool reopen();

    private:
        std::string m_filename;     //写入文件
        std::ofstream m_ofstream;   //stream流
    };

    //实例化内容
    class MassageFormatItem : public LogFormotter::FormatItem {
    public:
        MassageFormatItem(const std::string basicString="") {

        }
        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getContent();
        }
    };

    //实例化级别
    class LevelFormatItem : public LogFormotter::FormatItem {
    public:
        LevelFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << LogLevel::ToString(level);
        }
    };

    //  /n
    class EndlFormatItem : public LogFormotter::FormatItem {
    public:
        EndlFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << std::endl;
        }
    };

    //启动时间
    class ElapseFormatItem : public LogFormotter::FormatItem {
    public:
        ElapseFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getElapse();
        }
    };

    //日志器名称
    class NameFormatItem : public LogFormotter::FormatItem {
    public:
        NameFormatItem(const std::string basicString = "") {
        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << logger->getName();
        }
    };

    //进程号
    class ThreadFormatItem : public LogFormotter::FormatItem {
    public:
        ThreadFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getThreadId();
        }
    };

    //协程号
    class FiberFormatItem : public LogFormotter::FormatItem {
    public:
        FiberFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getFiberId();
        }
    };

    //时间
    class DateTimeFormatItem : public LogFormotter::FormatItem {
    public:
        DateTimeFormatItem(const std::string &format) :
                datatime(format) {
            if (datatime.empty()) {
                datatime = "%Y-%m-%d %H:%M:%S";
            }
        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time1 = event->getTime();
            localtime_r(&time1, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), datatime.c_str(), &tm);
            on << buf<<" ";
        }

    private:
        std::string datatime;
    };

    //文件名
    class FileNameFormatItem : public LogFormotter::FormatItem {
    public:
        FileNameFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getFile();
        }
    };

    //行号
    class LineFormatItem : public LogFormotter::FormatItem {
    public:
        LineFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getLine();
        }
    };

    //空格
    class SpaceFormatItem : public LogFormotter::FormatItem {
    public:
        SpaceFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << " ";
        }
    };

    //--》
    class StringFormatItem : public LogFormotter::FormatItem {
    public:
        StringFormatItem(const std::string &str) :
                m_string(str) {

        }

        void format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << m_string;
        }

    private:
        std::string m_string;
    };

    class LoggerManager{
    public:
        LoggerManager();
        //通过配置文件初始化日志器管理
        void init();
        Logger::ptr getLogger(std::string & name);
        Logger::ptr GetRoot()const{return m_root;}
    private:
        std::map<std::string,Logger::ptr>m_logger;
        Logger::ptr m_root;
    };
    typedef hh::Singleton<LoggerManager> LoggerMgr;
};
#endif //HH_LOG_H
