////
//// Created by 35148 on 2024/5/17.
////
#include "thread.h"
#include <iostream>
#include "log.h"
#include "config.h"

hh::RWMutex g_mutex;
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
int num = 0;
hh::Logger::ptr it;
hh::Mutex g_mutex2;
hh::NullMutex g_mutex3;

void fun1() {
//    for (int i = 0; i < 100000; i++){
//        hh::NullMutex::Lock lock(g_mutex3);
//        num++;
//    }
    while (true) {
        HH_LOG_INFO(it, "num = ***************************************************" + std::to_string(num++));
    }
}
void fun2() {
//    for (int i = 0; i < 100000; i++){
//        hh::NullMutex::Lock lock(g_mutex3);
//        num++;
//    }
    while (true) {
        HH_LOG_INFO(it, "num =====================================================" + std::to_string(num++));
    }
}
int main() {
    HH_LOG_INFO(g_logger, "pthread start");
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
    hh::Config::loadFromYaml(root);
    it = HH_LOG_NAME("root");
    std::vector<hh::Thread::ptr> p;
    for (int i = 0; i < 5; i++) {
        hh::Thread::ptr t(new hh::Thread(&fun1, "thread_" + std::to_string(i)));
        hh::Thread::ptr ts(new hh::Thread(&fun2, "thread_" + std::to_string(i)));
        p.push_back(t);
        p.push_back(ts);
    }
    for (size_t i = 0; i < p.size(); ++i) {
        p[i]->join();
    }
    std::cout << num << std::endl;


    return 0;
}

