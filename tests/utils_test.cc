//
// Created by 35148 on 2024/5/23.
//
#include "hh.h"
int main(){
    std::cout<<"hello world"<<std::endl;
    std::cout<<hh::BacktraceToString(10,1);
//    HH_ASSERT(0);
    HH_ASSERT2(0,"1231");
    return 0;
}