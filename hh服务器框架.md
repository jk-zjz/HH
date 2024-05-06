# HH

## 开发环境
centos7 linux  
gcc 9.1.0  
Cmake
## 项目结构
bin  -- 二进制文件目录  
build  -- 中间文件路径  
cmake  --  cmake文件路径  
CMakeLists.txt  --  cmake的定义文件  
hh  --  源文件路径  
tests  --  测试代码
## 日志系统
1）
    log4j

    作用域 hh::

    Logger(定义日志类别)
        |
        |------Formotter(日志格式)
        |
        |------LogLevel(日志级别)
        |
        |------LogEvent(日志事件)
        |
        |------LogEventWrap(宏的日志包装类)
        |
    Appender(日志输出地)
2）
    log日志的使用
```c++
//初始化日志器
hh::Logger::ptr logger(new hh::Logger);
//更改日志格式器
logger->set_formotter(new hh::LogFormotter("%d 123123 %m"));
//添加日志输出地
logger->addAppender(hh::LogAppender::ptr(new hh::FileLogAppender("bbll.txt")));
//初始化事件
hh::LogEvent::ptr logEvent(new hh::LogEvent(logger,logger->getLevel(),__FILE__,__LINE__,0,0,0, time(0)));
//生成日志
logger->Log(hh::LogLevel::DEBUG,logEvent);
```
3）宏的使用
```c++
//第一种 宏使用
hh::Logger::ptr logger(new hh::Logger);
logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));
HH_LOG_INFO(logger, "bsbsbs");
    //bsbsbs  ---  2024-05-02  root 334311 jkjk 
HH_LOG_INFO(logger)<<"bsbsbs"  //这种方式

//第二种 宏使用
hh::Logger::ptr logger(new hh::Logger);
logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));
HH_LOG_FAT_INFO(logger,"sbsbahbdah  %s %d","OK",123);
    //2024-05-02  root 334311 jkjk sbsbahbdah  OK 123
```
3）全局单例模式的使用 & 宏
```c++
    auto i=hh::LoggerMgr::GetInstance()->getLogger(k);
    HH_LOG_INFO(i, "1223");
```
## 配置系统


## 协程库封装

## socket函数库开发

## http协议开发

## 分布式协议

## 推荐系统