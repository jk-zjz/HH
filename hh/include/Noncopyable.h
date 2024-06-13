//
// Created by 35148 on 2024/6/14.
//

#ifndef HH_NONCOPYABLE_H
#define HH_NONCOPYABLE_H
namespace hh {

/**
 * @brief 对象无法拷贝,赋值
 */
    class Noncopyable {
    public:
        /**
         * @brief 默认构造函数
         */
        Noncopyable() = default;

        /**
         * @brief 默认析构函数
         */
        ~Noncopyable() = default;

        /**
         * @brief 拷贝构造函数(禁用)
         */
        Noncopyable(const Noncopyable&) = delete;

        /**
         * @brief 赋值函数(禁用)
         */
        Noncopyable& operator=(const Noncopyable&) = delete;
    };

}

#endif //HH_NONCOPYABLE_H
