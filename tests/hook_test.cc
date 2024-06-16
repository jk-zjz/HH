//
// Created by 35148 on 2024/6/14.
//
#include "hook.h"
#include "log.h"
#include "iomanage.h"
static hh::Logger::ptr gs_logger = HH_LOG_ROOT();
void test01(){
    hh::IOManager iomanager(1);
    iomanager.schedule([](){
        sleep(2);
        HH_LOG_INFO(gs_logger,"1");
    });
    iomanager.schedule([](){
        sleep(7);
        HH_LOG_INFO(gs_logger,"2");
    });
}
int main(){
    test01();
    return 0;
}