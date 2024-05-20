////
//// Created by 35148 on 2024/5/17.
////
#include "thread.h"
#include <iostream>
#include <pthread.h>
hh::RWMutex g_mutex;
hh::Logger::ptr g_logger = HH_LOG_ROOT();
int num = 0;
hh::Mutex g_mutex2;
void fun1() {
    std::cout << "fun1 " << hh::Thread::GetName() << "--"
              << hh::GetThreadID() << "---" << hh::Thread::GetThis()->get_id() << "--" <<
              hh::Thread::GetThis()->get_name() << std::endl;
    std::cout << hh::Thread::GetThis()->get_name() << std::endl;
    hh::Mutex::Lock lock(g_mutex2);
    for (int i = 0; i < 100000; ++i) {
        num++;
    }
}

int main() {
    HH_LOG_INFO(g_logger, "pthread start");
    std::vector<hh::Thread::ptr> pthreads;
    for (int i = 0; i < 5; i++) {
        hh::Thread::ptr t(new hh::Thread(&fun1, "name_" + std::to_string(i)));
        pthreads.push_back(t);

    }

    for (int i = 0; i < 5; i++) {
        pthreads[i]->join();
    }
    HH_LOG_INFO(g_logger, "pthread end");
    std::cout << num << std::endl;

    return 0;
}
