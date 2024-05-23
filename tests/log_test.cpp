#include <iostream>
#include "log.h"
#include "util.h"
int main() {
//
    hh::Logger::ptr logger(new hh::Logger);
    logger->setLevel(hh::LogLevel::UNKNOWN);
    logger->setFormatter(
            std::shared_ptr<hh::LogFormatter>(new hh::LogFormatter("%d{%Y-%m-%d} jk")));
   // logger->addAppender(hh::LogAppender::ptr(new hh::FileLogAppender("bbll.txt")));
    logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));

    HH_LOG_INFO(logger, "bsbsbs");
    hh::LogEvent::ptr logEvent(new hh::LogEvent(logger, logger->getLevel(),
                                                __FILE__, __LINE__,
                                                0, hh::GetThreadID(),
                                                hh::GetFiberID(), time(0)));
    HH_LOG_FAT_INFO(logger, "sbsbahbdah  %s %d", "OK", 123);
    logger->Log(hh::LogLevel::DEBUG,logEvent);
    const char *k("asdasd");
    auto i=hh::LoggerMgr::GetInstance()->getLogger(k);


    HH_LOG_LEVEL_CHAIN(logger,hh::LogLevel::UNKNOWN)<<"  asdas"<<"sadasd 00890978";

    std::cout<<hh::LogLevel::ToString(hh::LogLevel::DEBUG);
    return 0;
}
