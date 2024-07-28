//
// Created by 35148 on 2024/7/27.
//
#include "uri.h"
#include <iostream>
int main(int argc , char ** argv){
    hh::http::Uri::ptr uri = hh::http::Uri::Create("http://asd@www.baidu.com:89/s");

    std::cout<<uri->toString()<<std::endl;
    auto  i =uri->createAddress();
    if(i == nullptr){
        std::cout<<"nullptr"<<std::endl;
        return 0;
    }
    std::cout<<*i;
    return 0;
}