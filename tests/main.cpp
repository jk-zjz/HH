#include <iostream>
#include "log.h"
#include "util.h"
#include "config.h"
int main() {
//
    hh::Logger::ptr logger(new hh::Logger);
    logger->setLevel(hh::LogLevel::UNKNOW);
    logger->set_formotter(
    std::shared_ptr<hh::LogFormotter>(new hh::LogFormotter("%d{%Y-%m-%d} %b %c %b %t %b jkjk")));
   // logger->addAppender(hh::LogAppender::ptr(new hh::FileLogAppender("bbll.txt")));
    logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));

    HH_LOG_INFO(logger, "bsbsbs");
    hh::LogEvent::ptr logEvent(new hh::LogEvent(logger, logger->getLevel(),
                                                __FILE__, __LINE__,
                                                0, hh::GetThreadID(),
                                                hh::GetFiberID(), time(0)));
    HH_LOG_FAT_INFO(logger, "sbsbahbdah  %s %d", "OK", 123);
    logger->Log(hh::LogLevel::DEBUG,logEvent);
    std::string k("asdasd");
    auto i=hh::LoggerMgr::GetInstance()->getLogger(k);


    HH_LOG_LEVEL_CHAIN(logger,hh::LogLevel::UNKNOW)<<"  asdas"<<"sadasd 00890978";

    std::cout<<hh::LogLevel::ToString(hh::LogLevel::DEBUG);
    return 0;
}
