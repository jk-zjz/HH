//
// Created by 35148 on 2024/5/23.
//
#include "hh.h"
#include "fiber.h"
hh::Logger::ptr g_logger = HH_LOG_ROOT();
void func1(){
    HH_LOG_INFO(g_logger,"RUN_IN_ BEGIN");
    hh::Fiber::YieldToHold();
    HH_LOG_INFO(g_logger,"RUN_IN_ END");
    hh::Fiber::YieldToHold();
    HH_LOG_INFO(g_logger,"RUN_IN_ END")
}
int main(){
    {
        hh::Fiber::GetThis();
        HH_LOG_INFO(g_logger, "main begin")
        hh::Fiber::ptr f1(new hh::Fiber(func1));
        f1->swapIn();
        HH_LOG_INFO(g_logger, "main swapIn")
        f1->swapIn();
        HH_LOG_INFO(g_logger,"{main end")
        f1->swapIn();
    }
    HH_LOG_INFO(g_logger,"main end")
    return 0;
}