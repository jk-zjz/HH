//
// Created by 35148 on 2024/6/23.
//

#ifndef HH_ENDIAN_H
#define HH_ENDIAN_H

#define HH_LITTLE_ENDIAN 1
#define HH_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>

extern "C++" {
#include <type_traits>
namespace hh {
    // 模板函数byteswap，当T类型的大小为8字节（即uint64_t）时启用
    template<class T>
    typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
    byteswap(T value) {
        // 对于8字节类型，调用bswap_64函数进行字节序转换，并强制类型转换回T类型返回
        return (T) bswap_64((uint64_t) value);
    }

    // 模板函数byteswap，当T类型的大小为4字节（即uint32_t）时启用
    template<class T>
    typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
    byteswap(T value) {
        // 对于4字节类型，调用bswap_32函数进行字节序转换，并强制类型转换回T类型返回
        return (T) bswap_32((uint32_t) value);
    }

    // 模板函数byteswap，当T类型的大小为2字节（即uint16_t）时启用
    template<class T>
    typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
    byteswap(T value) {
        // 对于2字节类型，调用bswap_16函数进行字节序转换，并强制类型转换回T类型返回
        return (T) bswap_16((uint16_t) value);
    }

#if BYTE_ORDER == BIG_ENDIAN
#define HH_BYTE_ORDER HH_BIG_ENDIAN
#else
#define HH_BYTE_ORDER HH_LITTLE_ENDIAN
#endif

#if HH_BYTE_ORDER == HH_BIG_ENDIAN

/**
 * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
 */
    template<class T>
    T byteswapOnLittleEndian(T t) {
        return byteswap(t);
    }

/**
 * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
 */
    template<class T>
    T byteswapOnBigEndian(T t) {
       return t;
    }

#else

    /**
 * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
 */
template<class T>
T byteswapOnLittleEndian(T t) {
       return t;

}

/**
 * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
 */
template<class T>
T byteswapOnBigEndian(T t) {
     return byteswap(t);
}
#endif
}
}
#endif //HH_ENDIAN_H
