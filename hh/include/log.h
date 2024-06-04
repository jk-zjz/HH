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
#include "thread.h"
#define HH_LOG_LEVEL(logger,level,Str)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0),hh::Thread::GetName()))).setSS(Str)

//输出格式如下     Str----logger
#define HH_LOG_DEBUG(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::DEBUG,Str);
#define HH_LOG_INFO(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::INFO,Str);
#define HH_LOG_WARN(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::WARN,Str);
#define HH_LOG_ERROR(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::ERROR,Str);
#define HH_LOG_FATAL(logger,Str) HH_LOG_LEVEL(logger,hh::LogLevel::FATAL,Str);

//输出格式如下     logger 自定义内容,但是需传入级别
#define HH_LOG_LEVEL_CHAIN(logger,level)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0),hh::Thread::GetName()))).getEvent()->getSS()

#define HH_LOG_FAT_LEVEL(logger,level,fmt,...)   \
        if(logger->getLevel() <= level) \
            hh::LogEventWrap(hh::LogEvent::ptr(new hh::LogEvent(logger,level,__FILE__,__LINE__, \
            0,hh::GetThreadID(),hh::GetFiberID(), time(0),hh::Thread::GetName()))).getEvent()->format(fmt,__VA_ARGS__)

//输出格式如下     logger 自定义内容,但是不是使用链式的方式写入<<
#define HH_LOG_FAT_DEBUG(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::DEBUG,fmt,__VA_ARGS__);
#define HH_LOG_FAT_INFO(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::INFO,fmt,__VA_ARGS__);
#define HH_LOG_FAT_WARN(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::WARN,fmt,__VA_ARGS__);
#define HH_LOG_FAT_ERROR(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::ERROR,fmt,__VA_ARGS__);
#define HH_LOG_FAT_FATAL(logger,fmt,...) HH_LOG_FAT_LEVEL(logger,hh::LogLevel::FATAL,fmt,__VA_ARGS__);

//获取默认日志器方便
#define HH_LOG_ROOT() hh::LoggerMgr::GetInstance()->GetRoot()
//通过name获取格式器
#define HH_LOG_NAME(name) hh::LoggerMgr::GetInstance()->getLogger(name)

//可设置宏
#define HH_LOG_SET(name,logger) hh::LoggerMgr::GetInstance()->setlogger(name,logger)
//声明命名空间，以免名称重复，可 :: 访问
namespace hh {
    class Logger;
    //日志级别
    class LogLevel {
    public:
        //日志枚举
        enum Level {
            UNKNOWN = 0,//异常
            INFO = 1, //一般信息
            DEBUG = 2,//调试信息
            WARN = 3, //不会影响查询正常运行信息，警告
            ERROR = 4,//错误
            FATAL = 5 //严重错误
        };
        //静态获取传入日志
        static const char *ToString(LogLevel::Level level);
        static LogLevel::Level FromString(const std::string &str);
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
                 uint64_t mTime,
                 std::string mThreadName);
        //get方法
        const char *getFile() const { return m_file; }

        uint32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        uint32_t getFiberId() const { return m_fiberId; }

        std::string getContent() const { return m_ss.str(); }

        uint64_t getTime() const { return m_time; }

        std::stringstream &getSS() { return m_ss; }
        void setSS(const std::string &str) { m_ss<<str;}

        std::shared_ptr<Logger> getLogger()const {return m_logger;}
        LogLevel::Level getLevel()const {return m_level;}
        std::string getThreadName()const {return m_threadName;}
        void setThreadName(const std::string &name) { m_threadName = name; }

        //可变参函数   ("%s%d","qw",112)
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
        std::string m_threadName;         //线程名称

        std::shared_ptr<Logger> m_logger; //获取日志器
        LogLevel::Level m_level; //日志级别
    };
    /*
     * 日志事件管理类
     * 基本上给宏使用的
     * 通过LogEventWrap的析构函数产生log日志
     * m_logEvent存储了日志器--->通过调用日志器中的日志输出
     * */
    class LogEventWrap{
    public:
        LogEventWrap(LogEvent::ptr logEvent);
        //通过析构调用log日志器输出
        ~LogEventWrap();
        void setSS(const std::string& S);
        std::stringstream & getSS(){return m_logEvent->getSS();};
        LogEvent::ptr& getEvent(){return m_logEvent;}
    private:
        LogEvent::ptr m_logEvent;
    };

    /*
     * 日志格式器
     * 最终输出类
     * */
    class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        //通过输入的pattern 格式化m_items
        LogFormatter(const std::string &pattern);

        //格式化 event 返回
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        //获取是否为自定义格式器
        std::string get_pattern() const { return m_pattern; }

        //判断格式器初始化是否成功
        bool is_Error()const {return m_Error;}
    public:
        //格式输出基类
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            virtual ~FormatItem() {};
            //输出
            virtual void format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        //初始化格式到m_items
        void init();

    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
        bool m_Error= false;
    };

    //日志输入地(多个输出地-继承)
    class LogAppender {
        friend class Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef Mutex MutexType;
        void set_level(LogLevel::Level val);
        LogLevel::Level get_level()const {return m_level;}
        //多个输出地，需要虚析构，子类可释放
        virtual ~LogAppender() {};
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        //设置日志选器
        void setFormatter(LogFormatter::ptr val);
        void setFatherFormatter(LogFormatter::ptr val,bool type);
        LogFormatter::ptr getFormatter();

        //设置格式器的两种方式
        bool getFatherFormatter()const {return FatherFormatter;}
        void setFatherFormatter(bool val){FatherFormatter=val;}
    protected:
        LogFormatter::ptr m_Formatter;                      //日志格式器选择
        LogLevel::Level m_level = LogLevel::UNKNOWN;        //日志级别
        bool FatherFormatter=true;                          //true为默认，false为父亲
        MutexType m_mutex;                                  //互斥锁
    };
    //日志器--std::enable_shared_from_this<Logger>用于传递自己
    class Logger : public std::enable_shared_from_this<Logger> {
    friend class LoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef Mutex MutexType;
        Logger(const std::string &name = "root");

        void Log(LogLevel::Level level, LogEvent::ptr event);

        //日志输出函数
        void debug(LogEvent::ptr event);

        void info(LogEvent::ptr event);

        void warn(LogEvent::ptr event);

        void error(LogEvent::ptr event);

        void fatal(LogEvent::ptr event);

        //添加输出方式
        void addAppender(LogAppender::ptr appender);
        //删除输出方式
        void delectAppender(LogAppender::ptr appender);
        //清空
        void clearAppender();

        //设置(获取)什么日志记录
        LogLevel::Level getLevel() const { return m_level; }

        void setLevel(LogLevel::Level level) { m_level = level; }

        const std::string &getName() const { return m_name; }

        LogFormatter::ptr get_Formatter() const { return m_Formatter; }

        //设置格式器的方式
        void setFormatter(LogFormatter::ptr ptr1);
        void setFormatter(const std::string& formatter);

    private:
        std::string m_name;                         //谁初始化日志
        LogLevel::Level m_level;                    //日志级别
        std::list<LogAppender::ptr> m_appenders;    //日志输出地集合
        LogFormatter::ptr m_Formatter;              //日志格式器
        Logger::ptr m_root;                         //存放getlogger默认格式器地
        MutexType m_mutex;                          //锁
    };

    //输出控制台LogAppender
    class StdoutLogAppender : public LogAppender {
    public:
        std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    //输出到文件
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
        uint64_t m_lastTime = 0;    //检测文件是否被删除
    };

    //实例化内容
    class MassageFormatItem : public LogFormatter::FormatItem {
    public:
        MassageFormatItem(const std::string basicString="") {

        }
        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on <<" "<<event->getContent();
        }
    };

    //实例化级别 p
    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << LogLevel::ToString(level);
        }
    };

    //  /n
    class EndlFormatItem : public LogFormatter::FormatItem {
    public:
        EndlFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << std::endl;
        }
    };

    //启动时间 r
    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getElapse();
        }
    };

    //日志器名称 c
    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string basicString = "") {
        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            /*
             * 获取最终格式器名称
             * logenent 会被使用的格式器赋值，所以可以获取到最终格式器名称
             * */
            on << event->getLogger()->getName();
        }
    };

    //进程号t
    class ThreadFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getThreadId();
        }
    };

    //协程号 F
    class FiberFormatItem : public LogFormatter::FormatItem {
    public:
        FiberFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getFiberId();
        }
    };

    //时间d
    class DateTimeFormatItem : public LogFormatter::FormatItem {
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

    //文件名f
    class FileNameFormatItem : public LogFormatter::FormatItem {
    public:
        FileNameFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getFile();
        }
    };

    //行号l
    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << event->getLine();
        }
    };

    //空格b
    class SpaceFormatItem : public LogFormatter::FormatItem {
    public:
        SpaceFormatItem(const std::string basicString = "") {

        }

        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << " ";
        }
    };

    //--》C
    class StringFormatItem : public LogFormatter::FormatItem {
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
    class StringThreadNameItem : public LogFormatter::FormatItem {
    public:
        StringThreadNameItem(const std::string basicString = "") {
        }
        void
        format(std::shared_ptr<Logger> logger, std::ostream &on, LogLevel::Level level, LogEvent::ptr event) override {
            on << hh::Thread::GetName();
        }
    };
    /**
     * @log 日志器管理
     * */
    class LoggerManager{
    public:
        LoggerManager();
        //通过配置文件初始化日志器管理
        void init();
        typedef Mutex MutexType;
        Logger::ptr getLogger(const char *name);
        Logger::ptr GetRoot()const{return m_root;}
        std::map<std::string,Logger::ptr> getLoggers()const {return m_logger;}
        //设置日志器名称对应的日志器
        void setlogger(std::string key,Logger::ptr loggers){
            m_logger[key]=loggers;
        }
    private:
        std::map<std::string,Logger::ptr>m_logger;
        Logger::ptr m_root;
        MutexType m_mutex;
    };
    typedef hh::Singleton<LoggerManager> LoggerMgr;
};
#endif //HH_LOG_H
