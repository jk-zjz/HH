#include "hh.h"

static hh::Logger::ptr g_logger = HH_LOG_ROOT();

void run_in_fiber() {
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "run_in_fiber begin";
    hh::Fiber::YieldToHold();
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "run_in_fiber end";
    hh::Fiber::YieldToHold();
}

void test_fiber() {
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main begin -1";
    {
        hh::Fiber::GetThis();
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main begin";
        hh::Fiber::ptr fiber(new hh::Fiber(run_in_fiber));
        fiber->swapIn();
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main after swapIn";
        fiber->swapIn();
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main after end";
        fiber->swapIn();
    }
    HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::INFO) << "main after end2";
}

int main(int argc, char** argv) {
    hh::Thread::SetName("main");

    std::vector<hh::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i) {
        thrs.push_back(hh::Thread::ptr(
                new hh::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }
    return 0;
}