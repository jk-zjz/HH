//
// Created by 35148 on 2024/7/18.
//
#include "stream.h"
namespace hh{
    int Stream::readFixSize(void *buf, size_t size){
        size_t offset = 0;
        size_t left = size;
        while(left > 0){
            int len = read((char *)buf + offset, left);
            if(len <= 0){
                return len;
            }
            offset += len;
            left -= len;
        }
        return size;
    }
    int Stream::readFixSize(ByteArray::ptr ba, size_t size){
        size_t left = size;
        while(left > 0){
            int len = read(ba, left);
            if(len <= 0){
                return len;
            }
            left -= len;
        }
        return size;
    }
    int Stream::writeFixSize(const void *buf, size_t size){
        size_t offset = 0;
        size_t left = size;
        while(left > 0){
            int len = write((const char *)buf + offset, left);
            if(len <= 0){
                return len;
            }
            offset += len;
            left -= len;
        }
        return size;
    }
    int Stream::writeFixSize(ByteArray::ptr ba, size_t size){
        size_t left = size;
        while(left > 0){
            int len = write(ba, left);
            if(len <= 0){
                return len;
            }
            left -= len;
        }
        return size;
    }
}