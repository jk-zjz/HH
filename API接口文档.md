<div align='center' ><font size='60'>API接口文档</font></div>

## LOG日志接口

| LOG API                              | 说明           | 备注       | 有无重载 | 是否为虚   | 类是否为父类 | 父类                                   |
|--------------------------------------|--------------|----------|-----|--------|------|--------------------------------------|
| hh::Logger                           | 日志器类         |          |     |        | 否    | std::enable_shared_from_this<Logger> |
| hh::Logger::Log                      | 日志输出方法       | 输出       | 无   | 否      |
| hh::Logger::addAppender              | 日志输出添加方法     | 设置       | 无   | 否      |
| hh::logger::delectAppender           | 日志输出删除方法     | 设置       | 无   | 否      |
| hh::logger::clearAppender            | 日志输出清空方法     | 设置       | 无   | 否      |
| hh::logger::setLevel                 | 日志输出等级设置方法   | 设置       | 无   | 否      |
| hh::logger::setFileName              | 日志输出文件设置方法   | 设置       | 有   | 否      |
| hh::logger::getLevel                 | 日志输出等级获取方法   | 获取       | 无   | 否      |
| hh::logger::getName                  | 日志输出名称获取方法   | 获取       | 无   | 否      |
| hh::logger::get_Fromatter            | 日志输出格式获取方法   | 获取       | 无   | 否      |
| hh::LogAppender                      | 日志输出器类       |          |     |        | 是    |
| hh::LogAppender::log                 | 日志输出方法       | 输出       | 否   | 是      |
| hh::LogAppender::setFormatter        | 日志输出格式设置方法   | 设置       |
| hh::LogAppender::set_level           | 日志输出等级设置方法   | 设置       |
| hh::LogAppender::setFatherFormatter  | 设置是否为自定义格式   | 设置       |
| hh::LogAppender::setFatherFormatter  | 设置是否为自定义格式   | 设置       |
| hh::LogAppender::get_level           | 日志输出等级获取方法   | 获取       |
| hh::LogAppender::getFormatter        | 日志输出格式获取方法   | 获取       |
| hh::LogAppender::getFatherFormatter  | 日志输出格式获取方法   | 获取       |
| hh::LogFormatter                     | 日志输出格式类      |          |     |        | 否    |
| hh::LogFormatter::LogFormatter       | 通过格式字符串设置格式器 | 构造       |
| hh::LogFormatter::format             | 格式化日志输出方法    | 输出       |
| hh::LogFormatter::is_Error           | 判断是否为错误日志等级  | 判断       |
| hh::LogEventWrap                     | 日志事件管理类      |          |     |        | 否    |
| hh::LogEventWrap::LogEventWrap       | 日志事件管理类构造方法  | 构造       |     |        |      |
| hh::LogEventWrap::~LogEventWrap      | 日志析构输出方法     | 输出       |     |        |      |
| hh::LogEventWrap::setSS              | 设置日志输出流内容    | 设置       |     |        |      |
| hh::LogEventWrap::getSS              | 获取日志输出流内容    | 获取       |     |        |      |
| hh::LogEventWrap::getEvent           | 获取日志事件       | 获取       |     |        |      |
| hh::LogEvent                         | 日志事件类        |          |     |        | 否    |
| hh::LogEvent::setSS                  | 设置日志输出流内容    | 设置       |     |        |      |
| hh::LogEvent::format                 | 可变参数格式化方法内容  | 设置       | 有   |        |      |
| hh::LogEvent::setFormatter           | 设置日志格式器      | 设置       | 有   |        |      |
| hh::LogEvent::get_Formatter          | 获取日志格式器      | 获取       |     |        |      |
| hh::LogEvent::getFile                | 获取日志文件名      | 获取       |     |        |      |
| hh::LogEvent::Line                   | 获取日志行号       | 获取       |     |        |      |
| hh::LogEvent::getElapsed             | 获取日志时间       | 获取       |     |        |      |
| hh::LogEvent::getThreadId            | 获取日志线程号      | 获取       |     |        |      |
| hh::LogEvent::getFiberId             | 获取日志协程号      | 获取       |     |        |      |
| hh::LogEvent::getContent             | 获取日志内容       | 获取       |     |        |      |
| hh::LogEvent::getTime                | 获取日志时间       | 获取       |     |        |      |
| hh::LogEvent::getSS                  | 获取日志输出流内容    | 获取       |     |        |      |
| hh::LogEvent::getLevel               | 获取日志等级       | 获取       |     |        |      |
| hh::LogEvent::getLohger              | 获取日志器        | 获取       |     |        |      |
| hh::LogLevel                         | 日志等级类        |          | |        | |                                      |
| hh::LogLevel::ToString               |日志等级转字符串| 静态方法     | |        | |                                      |
| hh::LogLevel::FromString             |字符串转日志等级| 静态方法     | |        | |                                      |
| hh::FileLogAppender                  |日志输出器类|          | |        | | LogAppender                          |
| hh::FileLogAppender::FileLogAppender |日志输出器类构造方法| 构造       | |        | |          |
| hh::FileLogAppender::log             |日志输出方法| 输出       | | 虚方法的实现 | |          |
| hh::FileLogAppender::reopen          |日志文件重开| 设置       | |        | |          |
| hh::StdoutLogAppender                |日志输出器类|          | |        | | LogAppender        |
| hh::StdoutLogAppender::log           |日志输出方法| 输出       | | 虚方法的实现 | |          |
| hh::LoggerManager                    |日志管理器类| 基本为静态类返回 | |        | |          |
| hh::LoggerManager::LoggerMgr         |关键静态类|typedef hh::Singleton<LoggerManager> LoggerMgr| |        | |          |
| hh::LoggerManager::getLogger         |获取日志器| 获取       | |        | |          |
| hh::LoggerManager::GetRoot           |获取根日志器| 获取       | |        | |          |
| hh::LoggerManager::getLoggers        |获取日志器集合| 获取       | |        | |          |
| hh::LoggerManager::setLogger         |设置日志器| 设置       | |        | |          |

## config 配置API





