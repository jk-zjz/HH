//
// Created by 35148 on 2024/4/18.
//
#include "log.h"
#include "config.h"

namespace hh {

    const char *LogLevel::ToString(LogLevel::Level level) {
        //宏的返回传入参数，转字符串返回
        switch (level) {
#define XX(name) \
            case LogLevel::name: \
                return #name;     \
                break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef  XX
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
    }
/*Logger(日志器)  实现*/
//Logger 构造
    Logger::Logger(const std::string &name) :
            m_name(name),
            m_level(LogLevel::UNKNOW){
        //默认的日志格式器
        m_formotter.reset(new LogFormotter("%d <%t-%F> [%p:%m] <%f:%l> %b"));
    }

//添加输出地
    void Logger::addAppender(LogAppender::ptr appender) {
        if(!appender->getFormotter()){
            appender->setFormotter(m_formotter);
        }
        appender->set_level(m_level);
        m_appenders.push_back(appender);
    }

//删除输出地
    void Logger::delectAppender(LogAppender::ptr appender) {
        for (auto i = m_appenders.begin();
             i != m_appenders.end(); i++) {
            if (*i == appender) {
                m_appenders.erase(i);
                break;
            }
        }
    }

//设置每一个输入地
    void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            //大于日志级别
            auto p = shared_from_this();
            for (auto &i: m_appenders) {
                i->log(p,level, event);
            }
            std::cout<<std::endl;
        }
    }

//日志输出函数
    void Logger::debug(LogEvent::ptr event) {
        Log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        Log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        Log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        Log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        Log(LogLevel::FATAL, event);
    }

/*    FileLogAppender(日志输出地) 实现 */
    FileLogAppender::FileLogAppender(const std::string &filename)
            : m_filename(filename) {
    }

    bool FileLogAppender::reopen() {
        if (!m_ofstream.fail()) {
            m_ofstream.close();
        }
        m_ofstream.open(m_filename,std::ios::app);
        return !!m_ofstream;
    }

//文件输出地
    void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) {
        reopen();
        if (level >= m_level) {
            m_ofstream << m_formotter->format(logger,level, event)<<"\n";
        }
        m_ofstream.close();
    }

//控制台输出地
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formotter->format(logger,level, event);
        }
    }

/*LogFormotter(格式器) 实现*/
    LogFormotter::LogFormotter(const std::string &pattern)
            : m_pattern(pattern) {
            init();
    }

    std::string LogFormotter::format(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i: m_items) {
            i->format(logger,ss, level, event);
        }
        return ss.str();

    };

// 格式化输入->方法
// %s   %d{yyyy}
//m_pattern   %C %d{YYYY-MM-dd hh:mm:ss} [%m] %n
    void LogFormotter::init() {
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        bool type = false;
        for (size_t i = 0; i < m_pattern.size(); i++) {
            if (!type && isspace(m_pattern[i]) ){
                continue;
            }
            while (i<m_pattern.size() && m_pattern[i] != '%') {
                type = true;
                nstr.append(1, m_pattern[i]);
                i++;
            }
            if (!nstr.empty()) {
                if (nstr[nstr.size() - 1] == ' ') {
                    nstr.erase(nstr.size() - 1);
                }
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
                type = false;
                i--;
                continue;
            }
            std::string back_S;
            std::string end_S;

            int n = i + 1;
            if (static_cast<std::size_t>(n + 1) < m_pattern.size() && m_pattern[n] == '%' && m_pattern[n + 1] == '%'){
                // %%
                vec.push_back(std::make_tuple("%",std::string(),0));
            }
            else {
                if (static_cast<std::size_t>(n) < m_pattern.size()) {
                    back_S = m_pattern.substr(n, 1);
                    if (m_pattern[n + 1] == '{') {
                        n++;
                        while (static_cast<std::size_t>(n) < m_pattern.size() && m_pattern[n] != '}') {
                            if (m_pattern[n] == '%' && m_pattern[i+1]!='d') {
                                n--;
                                break;
                            }
                            n++;
                        }
                        if (m_pattern[n] == '}') {
                            end_S = m_pattern.substr(i + 3, n - i-3);
                            vec.push_back(std::make_tuple(back_S, end_S, 2));
                        }
                        else {
                            vec.push_back(std::make_tuple("<<error>>", std::string(), 0));
                        }
                    }
                    else {
                        vec.push_back(std::make_tuple(back_S, "", 1));
                    }
                }
                else {
                    vec.push_back(std::make_tuple(back_S, "", 1));
                }
                i = n;
            }
        }

/*
 * c 日志器名称
 * p 日志级别
 * r 启动时间
 * t 线程号
 * F 协程号
 * n 回车
 * d 时间
 * f 文件名称
 * l 行号
 * b 空格
 * C 自定义内容
 * */
//是一个lambda表达式通过传入的string 返回一个格式器类每一种格式对应一个输出方式
        static  std::map<std::string,std::function<LogFormotter::FormatItem::ptr(const std::string& str)> > s_format_times =
                {
#define XX(str,C)  \
        {#str,[](const std::string& fmt){return LogFormotter::FormatItem::ptr(new C(fmt));}}
        XX(m,NameFormatItem),
        XX(p,LevelFormatItem),
        XX(r,ElapseFormatItem),
        XX(t,ThreadFormatItem),
        XX(F,FiberFormatItem),
        XX(n,EndlFormatItem),
        XX(d,DateTimeFormatItem),
        XX(f,FileNameFormatItem),
        XX(l,LineFormatItem),
        XX(b,SpaceFormatItem),
        XX(C,MassageFormatItem)
#undef XX
        };
        for(auto & i : vec){
            if(std::get<2>(i) == 0){
                //字符串
                m_items.push_back(LogFormotter::FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }else{
                auto it = s_format_times.find(std::get<0>(i));
                if(it==s_format_times.end()){
                    //error
                    m_items.push_back(LogFormotter::FormatItem::ptr(new StringFormatItem("<<error_format %" +std::get<0>(i) + ">>")));
                }else{
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }
        }
        m_items.push_back(FormatItem::ptr(new MassageFormatItem()));
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *mFile,
                       uint32_t mLine, uint32_t mElapse, uint32_t mThreadId, uint32_t mFiberId,
                       uint64_t mTime) :
                       m_file(mFile), m_line(mLine), m_elapse(mElapse),
                       m_threadId(mThreadId), m_fiberId(mFiberId),
                       m_time(mTime),m_logger(std::move(logger)),
                       m_level(level){}

    void LogEvent::format(const char *fat, ...) {
        //绑定可变参
        va_list va;
        va_start(va,fat);
        format(fat,va);
        va_end(va);
    }

    void LogEvent::format(const char *fat, va_list al) {
        char * buf= nullptr;
        //根据大小动态开辟空间
        int len = vasprintf(&buf,fat,al);
        if(len != -1){
            //写入ss流存储
            m_ss<<std::string(buf,len);
            free(buf);
            buf= nullptr;
        }
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr logEvent):
            m_logEvent(std::move(logEvent)){
    }

    LogEventWrap::~LogEventWrap() {
        m_logEvent->getLogger()->Log(m_logEvent->getLevel(),m_logEvent);
    }

    void LogEventWrap::setSS(const std::string& S) {
        std::cout<<S<<"  ---  ";
    }

    LoggerManager::LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    }

    void LoggerManager::init() {

    }

    Logger::ptr LoggerManager::getLogger(std::string &name) {
        auto it = m_logger.find(name);
        return it==m_logger.end()?m_root:it->second;
    }
};
