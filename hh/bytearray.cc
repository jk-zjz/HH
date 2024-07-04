//
// Created by 35148 on 2024/7/2.
//
#include "bytearray.h"
#include <string>
#include <cstring>

namespace hh {

    ByteArray::Node::Node(size_t s, ByteArray::Node *n) {
        n->next = new Node(s);
    }

    ByteArray::Node::~Node() {
        if (ptr) {
            delete[] ptr;
        }
    }

    ByteArray::Node::Node(size_t s) :
            next(nullptr), ptr(new char[s]), size(s) {
    }

    ByteArray::ByteArray(size_t base_size) :
            m_baseSize(base_size),
            m_position(0),
            m_capacity(m_baseSize),
            m_endian(HH_BIG_ENDIAN),
            m_size(0),
            m_root(new Node(m_baseSize)),
            m_cur(m_root) {}

    ByteArray::~ByteArray() {
        Node *tmp = m_root;
        while (tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
    }

    void ByteArray::writeFint8(int8_t value) {
        write(&value, sizeof(value));
    };

    void ByteArray::writeFuint16(uint16_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    void ByteArray::writeFint16(int16_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    void ByteArray::writeFuint32(uint32_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    void ByteArray::writeFint32(int32_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    void ByteArray::writeFuint64(uint64_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    void ByteArray::writeFint64(int64_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    static uint32_t EncodeZigzag32(int32_t v) {
        if (v < 0) {
            return (((uint32_t) (-v)) << 1) - 1;
        } else {
            return v << 1;
        }
    }

    void ByteArray::writeInt32(int32_t value) {
        writeUint32(EncodeZigzag32(value));
    };

    void ByteArray::writeUint32(uint32_t value) {
        uint8_t buf[5];
        uint8_t i = 0;
        while (value >= 0x80) {
            buf[i++] = (value & 0x7f) | 0x80;
            value >>= 7;
        }
        buf[i++] = value;
        write(buf, i);
    };
    static uint32_t DecodeZigzag32(uint32_t v){
        return (v >> 1) ^ (-(v & 1));
    }
    static uint64_t EncodeZigzag64(int64_t v) {
        if (v < 0) {
            return (((uint64_t) (-v)) << 1) - 1;
        } else {
            return v << 1;
        }
    }
    static uint64_t DecodeZigzag64( uint64_t v){
        return (v >> 1) ^ (-(v & 1));
    }

    void ByteArray::writeInt64(int64_t value) {
        writeUint64(EncodeZigzag64(value));
    };

    void ByteArray::writeUint64(uint64_t value) {
        uint8_t buf[10];
        uint8_t i = 0;
        while (value >= 0x80) {
            buf[i++] = (value & 0x7f) | 0x80;
            value >>= 7;
        }
        buf[i++] = value;
        write(buf, i);
    };

    void ByteArray::writeFloat(float value) {
        uint32_t v;
        memcpy(&v, &value, sizeof(value));
        writeFuint32(v);
    };

    void ByteArray::writeDouble(double value) {
        uint64_t v;
        memcpy(&v, &value, sizeof(value));
        writeFuint64(v);
    };

    void ByteArray::writeStringF16(const std::string &value) {
        writeFuint16(value.size());
        write(value.c_str(), value.size());
    };

    void ByteArray::writeStringF32(const std::string &value) {
        writeFuint32(value.size());
        write(value.c_str(), value.size());
    };

    void ByteArray::writeStringF64(const std::string &value) {
        writeFuint64(value.size());
        write(value.c_str(), value.size());
    };

    void ByteArray::writeStringVint(const std::string &value) {
        writeUint64(value.size());
        write(value.c_str(), value.size());
    };

    void ByteArray::writeStringWithoutLength(const std::string &value) {};

    int8_t ByteArray::readFint8() {
        int8_t v = 0;
        read(&v, sizeof(v));
        return v;
    };

    uint8_t ByteArray::readFuint8() {
        uint8_t v = 0;
        read(&v, sizeof(v));
        return v;
    };
#define XX(type) \
    type v;      \
    read(&v, sizeof(v)); \
    if(m_endian == HH_BYTE_ORDER){ \
        return v;             \
    }else{       \
        return byteswap(v);             \
    }
    uint16_t ByteArray::readFuint16() {
        XX(uint16_t);
    };

    int16_t ByteArray::readFint16() {
        XX(int16_t);
    };

    uint32_t ByteArray::readFuint32() {
        XX(uint32_t);
    };

    int32_t ByteArray::readFint32() {
        XX(int32_t);
    };

    uint64_t ByteArray::readFuint64() {
        XX(uint64_t);
    };

    int64_t ByteArray::readFint64() {
        XX(int64_t);
    };
#undef XX

    int32_t ByteArray::readInt32() {
        return DecodeZigzag32(readUint32());
    };

    uint32_t ByteArray::readUint32() {
        uint32_t v = 0;
        for (int i = 0; i < 32; i += 7) {
            uint8_t b = readFuint8();
            if (b < 0x80) {
                v |= ((uint32_t) b) << i;
                break;
            } else {
                v |= ((((uint32_t) b) & 0x7f) << i);
            }
        }
        return v;
    };

    int64_t ByteArray::readInt64() {
        return DecodeZigzag64(readUint64());
    };

    uint64_t ByteArray::readUint64() {
        uint64_t v = 0;
        for (int i = 0; i < 64; i += 7) {
            uint8_t b = readFuint8();
            if (b < 0x80) {
                v |= ((uint64_t) b) << i;
                break;
            } else {
                v |= ((((uint64_t) b) & 0x7f) << i);
            }
        }
        return v;
    };

    float ByteArray::readFloat() {
        uint32_t v= readFuint32();
        float v1;
        memcpy(&v1, &v, sizeof(v));
        return v1;
    };

    double ByteArray::readDouble() {
        uint64_t v  = readFuint64();
        double v1;
        memcpy(&v1, &v, sizeof(v));
        return v1;
    };

    std::string ByteArray::readStringF16() {
        uint16_t len = readFuint16();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    std::string ByteArray::readStringF32() {
        uint32_t len = readFuint32();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    std::string ByteArray::readStringF64() {
        uint64_t len = readFuint64();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };
    std::string ByteArray::readStringVint() {
        uint64_t len = readUint64();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };


    void clear(){

    };

    void ByteArray::write(const void *buf, size_t size) {};

    void ByteArray::read(void *buf, size_t size) {};

    void ByteArray::writeToFile(const std::string &name) const {};

    void ByteArray::readFromFile(const std::string &name) {};

    void ByteArray::setPosition(size_t v) {}

    void ByteArray::addCapacity(size_t value) {}

    bool ByteArray::isLittleEndian() const {
        return m_endian != HH_BIG_ENDIAN;
    }

    void ByteArray::setLittleEndian(bool value) {
        m_endian = value ? HH_LITTLE_ENDIAN : HH_BIG_ENDIAN;
    }

    void ByteArray::writeFuint8(uint8_t value) {
        write(&value, sizeof(value));
    }

}