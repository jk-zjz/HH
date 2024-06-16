//
// Created by 35148 on 2024/6/3.
//
#include "scheduler.h"
#include <vector>
#include "log.h"

hh::Mutex g_mutex;
static hh::Logger::ptr g_logger = HH_LOG_ROOT();

void test_fiber() {
    static int s_count = 100;
    hh::Mutex::Lock lock(g_mutex);
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "test in fiber s_count=" << s_count;

    if (--s_count >= 0) {
        hh::Scheduler::GetThis()->schedule(&test_fiber);
    }
}

void fun23() {
    static int sasd = 100;

    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "************=" << sasd;

    if (--sasd >= 0) {
        hh::Scheduler::GetThis()->schedule(&fun23);
    }
}

struct A {
    int a = 0;
};

int main(int argc, char **argv) {
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main";
    hh::Scheduler sc(2, true, "test");
    sc.start();
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "schedule";
    sc.schedule(&test_fiber);
    sc.schedule(&fun23);
    sc.stop();
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "over";
//    hh::Scheduler sc;
//    sc.start();
//    sc.stop();
    return 0;
}