//
// Created by 35148 on 2024/7/27.
//
#include "uri.h"
int main(int argc , char ** argv){
    hh::http::Uri::ptr uri = hh::http::Uri::Create("http://asd@www.baidu.com:80/index.html?id=100&name=hh#frg");
    return 0;
}