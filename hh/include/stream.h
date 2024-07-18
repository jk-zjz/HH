//
// Created by 35148 on 2024/7/18.
//

#ifndef HH_STREAM_H
#define HH_STREAM_H
#include <memory>
#include "bytearray.h"
namespace hh{
    class Stream{
    public:
        typedef std::shared_ptr<Stream> ptr;
        virtual ~Stream(){}
        virtual int read(void *buf, size_t size) = 0;
        virtual int read(ByteArray::ptr ba, size_t size) = 0;
        virtual int write(const void *buf, size_t size) = 0;
        virtual int write(ByteArray::ptr ba, size_t size) = 0;

        virtual int readFixSize(void *buf, size_t size);
        virtual int readFixSize(ByteArray::ptr ba, size_t size);
        virtual int writeFixSize(const void *buf, size_t size);
        virtual int writeFixSize(ByteArray::ptr ba, size_t size);
        virtual void close() = 0;
    };
}
#endif //HH_STREAM_H
