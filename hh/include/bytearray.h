//
// Created by 35148 on 2024/7/2.
//

#ifndef HH_BYTEARRAY_H
#define HH_BYTEARRAY_H

#include <memory>

namespace hh {
    // 带F表示不进行压缩
    class ByteArray {
    public:
        typedef std::shared_ptr<ByteArray> ptr;

        struct Node {
            Node *next;
            char *ptr;
            size_t size;

            Node(size_t s);

            Node(size_t s, Node *n);

            ~Node();
        };

        ByteArray(size_t base_size = 4096);

        ~ByteArray();

        // write 中写入固定的大小类容 int  和 uint
        void writeFint8(int8_t value);

        void writeFuint8(uint8_t value);

        void writeFuint16(uint16_t value);

        void writeFint16(int16_t value);

        void writeFuint32(uint32_t value);

        void writeFint32(int32_t value);

        void writeFuint64(uint64_t value);

        void writeFint64(int64_t value);

        // 进行压缩的函数
        void writeInt32( int32_t value);

        void writeUint32( uint32_t value);

        void writeInt64( int64_t value);

        void writeUint64( uint64_t value);

        //写入浮点数
        void writeFloat( float value);

        void writeDouble( double value);

        // 写入字符串
        void writeStringF16(const std::string &value);

        void writeStringF32(const std::string &value);

        void writeStringF64(const std::string &value);

        // 写入变长压缩
        void writeStringVint(const std::string &value);

        void writeStringWithoutLength(const std::string &value);

        // read 读取固定大小类容 int  和 uint
        int8_t readFint8();
        uint8_t readFuint8();

        uint16_t readFuint16();

        int16_t readFint16();

        uint32_t readFuint32();

        int32_t readFint32();

        uint64_t readFuint64();

        int64_t readFint64();

        // 读取压缩的函数
        int32_t readInt32();

        uint32_t readUint32();

        int64_t readInt64();

        uint64_t readUint64();

        // 读取浮点数
        float readFloat();

        double readDouble();

        // 读取字符串
        std::string readStringF16();

        std::string readStringF32();

        std::string readStringF64();

        std::string readStringVint();
        // 其他操作

        // 清空
        void clear();

        // 读写
        void write(const void *buf, size_t size);

        void read(void *buf, size_t size);

        // 读写文件
        void writeToFile(const std::string &name) const;

        void readFromFile(const std::string &name);

        // 获取位置
        size_t getPosition() const { return m_position; }

        void setPosition(size_t v);

        // 获取大小
        size_t getBaseSize() const { return m_baseSize; }

        // 获取剩余大小
        size_t getRemainSize() const { return m_capacity - m_position; }

        bool isLittleEndian() const;

        void setLittleEndian(bool value = true);

    private:
        void addCapacity(size_t value);

        size_t getCapacity() const { return m_capacity; }

    private:
        size_t m_baseSize;  // 基础大小
        size_t m_position;  // 位置
        size_t m_capacity;  // 分配容量
        int8_t m_endian;    // 大小端
        size_t m_size;      // 实际容量
        Node *m_root;       // 根节点
        Node *m_cur;        // 当前节点
    };

}
#endif //HH_BYTEARRAY_H
