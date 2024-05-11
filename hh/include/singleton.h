//
// Created by 35148 on 2024/5/4.
//
#ifndef HH_SINGLETON_H
#define HH_SINGLETON_H
#include <iostream>
#include <memory>

namespace hh{
    //单例模式
    template<class T,class X=void,int N=0>
    class Singleton{
    public:
        static T* GetInstance(){
            static T v;
            return &v;
        }
    };
    template<class T,class X=void,int N=0>
    class Singletonptr{
        static std::shared_ptr<T>GetInstance(){
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };
}
#endif //HH_SINGLETON_H
