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

    LogLevel::Level LogLevel::FromString(const std::string &str) {
#define XX(name) \
    if(str==#name){ \
        return LogLevel::name; \
    }
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        return LogLevel::UNKNOW;
    }
/*Logger(日志器)  实现*/
//Logger 构造
    Logger::Logger(const std::string &name) :
            m_name(name),
            m_level(LogLevel::UNKNOW) {
        //默认的日志格式器
        m_formotter.reset(new LogFormotter("%d <%t-%F> [%p:%m] <%f:%l> %b"));
    }

//添加输出地
    void Logger::addAppender(LogAppender::ptr appender) {
        if (!appender->getFormotter()) {
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
            //大于日志级别---判断输出地是否为空
            if (!m_appenders.empty()) {
                auto p = shared_from_this();
                for (auto &i: m_appenders) {
                    i->log(p, level, event);
                }
            } else if (m_root) {
                m_root->Log(level, event);
            }
            std::cout << std::endl;
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

    void Logger::clearAppender() {
        m_appenders.clear();
    }

    void Logger::setFormatter(const std::string &formatter) {
        //防止格式器为错误的
        hh::LogFormotter::ptr new_Val(new hh::LogFormotter(formatter));
        if (new_Val->is_Error()) {
            std::cout << "Formatter error " << formatter << " init error" << std::endl;
            return;
        }
        m_formotter = new_Val;
    }

/*    FileLogAppender(日志输出地) 实现 */
    FileLogAppender::FileLogAppender(const std::string &filename)
            : m_filename(filename) {
    }

    bool FileLogAppender::reopen() {
        if (!m_ofstream.fail()) {
            m_ofstream.close();
        }
        m_ofstream.open(m_filename, std::ios::app);
        return !!m_ofstream;
    }

//文件输出地
    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        reopen();
        if (level >= m_level) {
            m_ofstream << m_formotter->format(logger, level, event) << "\n";
        }
        m_ofstream.close();
    }

//控制台输出地
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formotter->format(logger, level, event);
        }
    }

/*LogFormotter(格式器) 实现*/
    LogFormotter::LogFormotter(const std::string &pattern)
            : m_pattern(pattern) {
        init();
    }

    std::string LogFormotter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i: m_items) {
            i->format(logger, ss, level, event);
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
            if (!type && isspace(m_pattern[i])) {
                continue;
            }
            while (i < m_pattern.size() && m_pattern[i] != '%') {
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
            if (static_cast<std::size_t>(n + 1) < m_pattern.size() && m_pattern[n] == '%' && m_pattern[n + 1] == '%') {
                // %%
                vec.push_back(std::make_tuple("%", std::string(), 0));
            } else {
                if (static_cast<std::size_t>(n) < m_pattern.size()) {
                    back_S = m_pattern.substr(n, 1);
                    if (m_pattern[n + 1] == '{') {
                        n++;
                        while (static_cast<std::size_t>(n) < m_pattern.size() && m_pattern[n] != '}') {
                            if (m_pattern[n] == '%' && m_pattern[i + 1] != 'd') {
                                n--;
                                break;
                            }
                            n++;
                        }
                        if (m_pattern[n] == '}') {
                            end_S = m_pattern.substr(i + 3, n - i - 3);
                            vec.push_back(std::make_tuple(back_S, end_S, 2));
                        } else {
                            vec.push_back(std::make_tuple("<<error>>", std::string(), 0));
                            m_Error = true;
                        }
                    } else {
                        vec.push_back(std::make_tuple(back_S, "", 1));
                    }
                } else {
                    vec.push_back(std::make_tuple(back_S, "", 1));
                }
                i = n;
            }
        }
/*
 * m 日志器名称
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
        static std::map<std::string, std::function<LogFormotter::FormatItem::ptr(
                const std::string &str)> > s_format_times =
                {
#define XX(str, C)  \
        {#str,[](const std::string& fmt){return LogFormotter::FormatItem::ptr(new C(fmt));}}
                        XX(m, NameFormatItem),
                        XX(p, LevelFormatItem),
                        XX(r, ElapseFormatItem),
                        XX(t, ThreadFormatItem),
                        XX(F, FiberFormatItem),
                        XX(n, EndlFormatItem),
                        XX(d, DateTimeFormatItem),
                        XX(f, FileNameFormatItem),
                        XX(l, LineFormatItem),
                        XX(b, SpaceFormatItem),
                        XX(C, MassageFormatItem)
#undef XX
                };
        for (auto &i: vec) {
            if (std::get<2>(i) == 0) {
                //字符串
                m_items.push_back(LogFormotter::FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_times.find(std::get<0>(i));
                if (it == s_format_times.end()) {
                    //error
                    m_items.push_back(LogFormotter::FormatItem::ptr(
                            new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_Error = true;
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }
        }
        m_items.push_back(FormatItem::ptr(new MassageFormatItem()));
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *mFile,
                       uint32_t mLine, uint32_t mElapse, uint32_t mThreadId, uint32_t mFiberId,
                       uint64_t mTime) :
            m_file(mFile), m_line(mLine), m_elapse(mElapse),
            m_threadId(mThreadId), m_fiberId(mFiberId),
            m_time(mTime), m_logger(std::move(logger)),
            m_level(level) {}

    void LogEvent::format(const char *fat, ...) {
        //绑定可变参
        va_list va;
        va_start(va, fat);
        format(fat, va);
        va_end(va);
    }

    void LogEvent::format(const char *fat, va_list al) {
        char *buf = nullptr;
        //根据大小动态开辟空间
        int len = vasprintf(&buf, fat, al);
        if (len != -1) {
            //写入ss流存储
            m_ss << std::string(buf, len);
            free(buf);
            buf = nullptr;
        }
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr logEvent) :
            m_logEvent(std::move(logEvent)) {
    }

    LogEventWrap::~LogEventWrap() {
        m_logEvent->getLogger()->Log(m_logEvent->getLevel(), m_logEvent);
    }

    void LogEventWrap::setSS(const std::string &S) {
        std::cout << S << "  ---  ";
    }

    LoggerManager::LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
        init();
    }

    class LogAppenderDefine {
    public:
        int type = 0;//1为file 2为stdout
        std::string file;
        std::string formatter;
        LogLevel::Level Level = hh::LogLevel::UNKNOW;

        bool operator==(const LogAppenderDefine &out) const {
            return type == out.type &&
                   file == out.file &&
                   formatter == out.formatter &&
                   Level == out.Level;
        }
    };

    class LogDefine {
    public:
        std::string name;
        LogLevel::Level Level = hh::LogLevel::UNKNOW;
        std::string formatter;
        std::vector<LogAppenderDefine> append;

        bool operator==(const LogDefine &out) const {
            return name == out.name &&
                   Level == out.Level &&
                   formatter == out.formatter &&
                   append == out.append;
        }

        bool operator<(const LogDefine &out) const {
            return name < out.name;
        }

        std::string to_string() const {
            std::stringstream ss;
            ss << "{";
            ss << "name:" << name << ",";
            ss << "level:" << LogLevel::ToString(Level) << ",";
            ss << "formatter:" << formatter << ",";
            ss << "{";
            for (auto &i: append) {
                ss << "{";
                ss << "type:" << i.type << ",";
                ss << "file:" << i.file << ",";
            }
            return ss.str();
        }
    };

    //特化类
    template<>
    class LexicalCast<std::string, std::set<LogDefine>> {
    public:
        std::set<LogDefine> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            std::set<LogDefine> sld;
            for (auto i: node) {
                if (!i["name"].IsDefined()) {
                    std::cout << "log config error: name is null, " << std::endl;
                    continue;
                }
                LogDefine ld;
                ld.name = i["name"].as<std::string>();
                ld.Level = i["level"].IsDefined() ?
                           hh::LogLevel::FromString(i["level"].as<std::string>()) :
                           hh::LogLevel::UNKNOW;
                if (i["formatter"].IsDefined())
                    ld.formatter = i["formatter"].as<std::string>();
                if (i["appender"].IsDefined()) {
                    for (auto it: i["appender"]) {
                        LogAppenderDefine lad;
                        if (!it["type"].IsDefined()) {
                            std::cout << "log config error: appender type is null, " << it << std::endl;
                            continue;
                        }
                        if (it["type"].as<std::string>() == "FileLogAppender") {
                            if (!it["file"].IsDefined()) {
                                std::cout << "log config error: appender file is null, " << it << std::endl;
                                continue;
                            }
                            lad.type = 1;
                            lad.Level = ld.Level;
                            lad.formatter = ld.formatter;
                            lad.file = it["file"].as<std::string>();
                        } else if (it["type"].as<std::string>() == "StdoutLogAppender") {
                            lad.type = 2;
                            lad.Level = ld.Level;
                            lad.formatter = ld.formatter;
                        } else {
                            std::cout << "log config error: appender type is invalid, " << it << std::endl;
                            continue;
                        }
                        ld.append.push_back(lad);
                    }
                }
                sld.insert(ld);
            }
            return sld;
        }
    };

    template<>
    class LexicalCast<std::set<LogDefine>, std::string> {
    public:
        std::string operator()(const std::set<LogDefine> &u) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &is: u) {
                YAML::Node name;
                name["name"] = is.name;
                name["level"] = LogLevel::ToString(is.Level);
                if (is.formatter.empty()) {
                    name["formatter"] = is.formatter;
                }
                for (auto &a: is.append) {
                    YAML::Node na;
                    if (a.type == 1) {
                        na["type"] = "FileLogAppender";
                        na["file"] = a.file;
                    } else if (a.type == 2) {
                        na["type"] = "StdoutLogAppender";
                    }
                    node["level"] = LogLevel::ToString(a.Level);
                    name["appender"].push_back(na);
                    if (a.formatter.empty()) {
                        na["formatter"] = a.formatter;
                    }
                }
                node.push_back(name);
            }
            ss << node;
            return ss.str();
        }
    };

    hh::ConfigVar<std::set<LogDefine>>::ptr var =
            hh::Config::Lookup("logs", std::set<LogDefine>(), "log define set");
    class LogIniter {
    public:
        LogIniter() {
            var->addOcb(0xF1E123, [](const std::set<LogDefine> &old_valuse,
                                     const std::set<LogDefine> &new_valuse) {
                HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(), hh::LogLevel::INFO) << "LogIniter init event trigger";
                std::cout<<"old_valuse:"<<old_valuse.size()<<std::endl;
                for (auto &ob: old_valuse) {
                    std::cout << ob.to_string() << std::endl;
                }
                std::cout<<"new_valuse:"<<new_valuse.size()<<std::endl;
                for (auto &nb: new_valuse) {
                    std::cout << nb.to_string() << std::endl;
                }
                //新增
                for (auto &i: new_valuse) {
                    hh::Logger::ptr logger;
                    std::string name(i.name);
                    auto it = old_valuse.find(i);
                    if (it == old_valuse.end()) {
                        //老的里面没有新的   新增
                        logger= HH_LOG_NAME(name);
                    } else if (!(*it == i)) {
                        //老的里面有并且不相同  更改
                        logger = HH_LOG_NAME(name);
                    }else{
                        continue;
                    }
                    logger->setLevel(i.Level);
                    if (!(i.formatter.empty())) {
                        logger->setFormatter(i.formatter);
                    }
                    logger->clearAppender();
                    for (auto &app: i.append) {
                        std::cout<<"1-";
                        hh::LogAppender::ptr logAppender;
                        if (app.type == 1) {
                            logAppender.reset(new FileLogAppender(app.file));
                        } else if (app.type == 2) {
                            logAppender.reset(new StdoutLogAppender);
                        }
                        logAppender->set_level(app.Level);
                        logger->addAppender(logAppender);
                    }
                    HH_LOG_SET(i.name,logger);
                }
                for (auto &i: old_valuse) {
                    auto it = new_valuse.find(i);
                    if (it == new_valuse.end()) {
                        //删除
                        std::string name(it->name);
                        hh::Logger::ptr logger = HH_LOG_NAME(name);
                        logger->setLevel((hh::LogLevel::Level) 100);
                        logger->clearAppender();
                        HH_LOG_SET(name,logger);
                    }
                }
            });
        };
    };

    static LogIniter _init_log;

    void LoggerManager::init() {
    }

    Logger::ptr LoggerManager::getLogger(std::string &name) {
        /*
         * 获取格式器时，判断是否查找
         * 未存在初始化格式器，使用传递格式器名称
         * 为新格式器赋值默认格式器用于输出，因为新格式器并不有输出地
         * */
        auto it = m_logger.find(name);
        if (it != m_logger.end()) {
            return it->second;
        }
        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_logger[name] = logger;
        return logger;
    }
};
