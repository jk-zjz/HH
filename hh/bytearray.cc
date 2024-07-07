//
// Created by 35148 on 2024/7/2.
//
#include "bytearray.h"
#include <string>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include "log.h"

namespace hh {
    static Logger::ptr g_logger = HH_LOG_NAME("system");

    /**
     * 构造在传入节点后
     * @param s 数据域大小
     * @param n 传入节点
     */
    ByteArray::Node::Node(size_t s, ByteArray::Node *n) {
        n->next = new Node(s);
    }

    /**
     * 释放节点数据域
     */
    ByteArray::Node::~Node() {
        if (ptr) {
            delete[] ptr;
        }
    }

    /**
     * 构造节点
     * @param s
     */
    ByteArray::Node::Node(size_t s) :
            next(nullptr), ptr(new char[s]), size(s) {
    }

    ByteArray::Node::Node()
            : next(nullptr), ptr(nullptr), size(0) {
    }

    ByteArray::ByteArray(size_t base_size)
            : m_baseSize(base_size), m_position(0), m_capacity(base_size), m_endian(HH_BIG_ENDIAN), m_size(0),
              m_root(new Node(base_size)), m_cur(m_root) {

    }

    ByteArray::~ByteArray() {
        Node *tmp = m_root;
        while (tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
    }

    /**
     * 不压缩写入8bit
     * @param value
     */
    void ByteArray::writeFint8(int8_t value) {
        write(&value, sizeof(value));
    };

    /**
     * 不压缩无符号16bit
     * @param value
     */
    void ByteArray::writeFuint16(uint16_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * 不压缩有符号16bit
     * @param value
     */
    void ByteArray::writeFint16(int16_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * 不压缩无符号32bit
     * @param value
     */
    void ByteArray::writeFuint32(uint32_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * 不压缩有符号32bit
     * @param value
     */
    void ByteArray::writeFint32(int32_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * 不压缩无符号64bit
     * @param value
     */
    void ByteArray::writeFuint64(uint64_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * 不压缩有符号64bt
     * @param value
     */
    void ByteArray::writeFint64(int64_t value) {
        if (m_endian != HH_BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    };

    /**
     * Zigag 压缩方式
     * @param v
     * @return
     */
    static uint32_t EncodeZigzag32(int32_t v) {
        if (v < 0) {
            return ((uint32_t) (-v)) * 2 - 1;
        } else {
            return v * 2;
        }
    }

    /**
     * 压缩有符号32bit
     * @param value
     */
    void ByteArray::writeInt32(int32_t value) {
        writeUint32(EncodeZigzag32(value));
    };

    /**
     * 压缩无符号32bit
     * 使用7bit压缩方式
     * @param value
     */
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

    /**
     * 解压32bit  Zigzag
     * @param v
     * @return
     */
    static uint32_t DecodeZigzag32(uint32_t v) {
        return (v >> 1) ^ (-(v & 1));
    }

    /**
     * 压缩62bit  Zigzag
     * @param v
     * @return
     */
    static uint64_t EncodeZigzag64(int64_t v) {
        if (v < 0) {
            return ((uint64_t) (-v)) * 2 - 1;
        } else {
            return v * 2;
        }
    }

    /**
     * 解压64bit  Zigzag
     * @param v
     * @return
     */
    static uint64_t DecodeZigzag64(uint64_t v) {
        return (v >> 1) ^ -(v & 1);
    }

    /**
     * 压缩有符号64bit
     * @param value
     */
    void ByteArray::writeInt64(int64_t value) {
        writeUint64(EncodeZigzag64(value));
    };

    /**
     * 压缩无符号64bit
     * @param value
     */
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

    /**
     * 转换浮点位32uint存储
     * @param value
     */
    void ByteArray::writeFloat(float value) {
        uint32_t v;
        memcpy(&v, &value, sizeof(value));
        writeFuint32(v);
    };

    /**
     * 转换浮点位64uint存储
     * @param value
     */
    void ByteArray::writeDouble(double value) {
        uint64_t v;
        memcpy(&v, &value, sizeof(value));
        writeFuint64(v);
    };

    /**
     * 压缩16大小的字符串
     * @param value
     */
    void ByteArray::writeStringF16(const std::string &value) {
        writeFuint16(value.size());
        write(value.c_str(), value.size());
    };

    /**
     * 压缩32大小的字符串
     * @param value
     */
    void ByteArray::writeStringF32(const std::string &value) {
        writeFuint32(value.size());
        write(value.c_str(), value.size());
    };

    /**
     * 压缩64大小的字符串
     * @param value
     */
    void ByteArray::writeStringF64(const std::string &value) {
        writeFuint64(value.size());
        write(value.c_str(), value.size());
    };

    /**
     * 压缩vint大小的字符串
     * @param value
     */
    void ByteArray::writeStringVint(const std::string &value) {
        writeUint64(value.size());
        write(value.c_str(), value.size());
    };

    /**
     * 不压缩字符串
     * @param value
     */
    void ByteArray::writeStringWithoutLength(const std::string &value) {
        write(value.c_str(), value.size());
    }

    /**
     * 读取8bit
     * @return
     */
    int8_t ByteArray::readFint8() {
        int8_t v = 0;
        read(&v, sizeof(v));
        return v;
    };

    /**
     * 读取8bit
     * @return
     */
    uint8_t ByteArray::readFuint8() {
        uint8_t v = 0;
        read(&v, sizeof(v));
        return v;
    };
    /**
     * 使用宏方式
     */
#define XX(type) \
    type v;      \
    read(&v, sizeof(v)); \
    if(m_endian == HH_BYTE_ORDER){ \
        return v;             \
    }else{       \
        return byteswap(v);             \
    }

    /**
     * 读取无符号16bit
     * @return
     */
    uint16_t ByteArray::readFuint16() {
        XX(uint16_t);
    };

    /**
     * 读取16bit
     * @return
     */
    int16_t ByteArray::readFint16() {
        XX(int16_t);
    };

    /**
     * 读取无符号32bit
     * @return
     */
    uint32_t ByteArray::readFuint32() {
        XX(uint32_t);
    };

    /**
     * 读取32bit
     * @return
     */
    int32_t ByteArray::readFint32() {
        XX(int32_t);
    };

    /**
     * 读取无符号64bit
     * @return
     */
    uint64_t ByteArray::readFuint64() {
        XX(uint64_t);
    };

    /**
     * 读取64bit
     * @return
     */
    int64_t ByteArray::readFint64() {
        XX(int64_t);
    };
#undef XX

    /**
     * 解压有符号32bit
     * @return
     */
    int32_t ByteArray::readInt32() {
        return DecodeZigzag32(readUint32());
    };

    /**
     * 读取无符号32bit
     * @return
     */
    uint32_t ByteArray::readUint32() {
        uint32_t v = 0;
        for (int i = 0; i < 32; i += 7) {
            uint8_t b = readFuint8();
            if (b < 0x80) {
                v |= ((uint32_t) b) << i;
                break;
            } else {
                v |= (((uint32_t) (b & 0x7f)) << i);
            }
        }
        return v;
    };

    /**
     * 解压有符号64bit
     * @return
     */
    int64_t ByteArray::readInt64() {
        return DecodeZigzag64(readUint64());
    };

    /**
     * 读取无符号64bit
     * @return
     */
    uint64_t ByteArray::readUint64() {
        uint64_t v = 0;
        for (int i = 0; i < 64; i += 7) {
            uint8_t b = readFuint8();
            if (b < 0x80) {
                v |= ((uint64_t) b) << i;
                break;
            } else {
                v |= (((uint64_t) (b & 0x7f)) << i);
            }
        }
        return v;
    };

    /**
     * 读取浮点数
     * @return
     */
    float ByteArray::readFloat() {
        uint32_t v = readFuint32();
        float v1;
        memcpy(&v1, &v, sizeof(v));
        return v1;
    };

    /**
     * 读取双精度浮点数
     * @return
     */
    double ByteArray::readDouble() {
        uint64_t v = readFuint64();
        double v1;
        memcpy(&v1, &v, sizeof(v));
        return v1;
    };

    /**
     * 读取16bit字符串
     * @return
     */
    std::string ByteArray::readStringF16() {
        uint16_t len = readFuint16();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    /**
     * 读取32bit字符串
     * @return
     */
    std::string ByteArray::readStringF32() {
        uint32_t len = readFuint32();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    /**
     * 读取64bit字符串
     * @return
     */

    std::string ByteArray::readStringF64() {
        uint64_t len = readFuint64();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    /**
     * 读取vint字符串
     * @return
     */
    std::string ByteArray::readStringVint() {
        uint64_t len = readUint64();
        std::string v;
        v.resize(len);
        read(&v[0], len);
        return v;
    };

    /**
     * 清空
     */
    void ByteArray::clear() {
        // 偏移位置与实际大小置为0
        m_position = m_size = 0;
        // 吧容量设置为基础大小
        m_capacity = m_baseSize;
        // 清空节点，保留root
        Node *tmp = m_root->next;
        while (tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
        m_cur = m_root;
        m_root->next = nullptr;
    }

    void ByteArray::write(const void *buf, size_t size) {
        if (size == 0) {
            return;
        }
        addCapacity(size);

        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        size_t bpos = 0;

        while (size > 0) {
            if (ncap >= size) {
                memcpy(m_cur->ptr + npos, (const char *) buf + bpos, size);
                if (m_cur->size == (npos + size)) {
                    m_cur = m_cur->next;
                }
                m_position += size;
                bpos += size;
                size = 0;
            } else {
                memcpy(m_cur->ptr + npos, (const char *) buf + bpos, ncap);
                m_position += ncap;
                bpos += ncap;
                size -= ncap;
                m_cur = m_cur->next;
                ncap = m_cur->size;
                npos = 0;
            }
        }

        if (m_position > m_size) {
            m_size = m_position;
        }
    }

    void ByteArray::read(void *buf, size_t size) {
        if (size > getRemainSize()) {
            throw std::out_of_range("ByteArray::read out of range");
        }
        // 写入位置
        size_t npos = m_position % m_baseSize;
        // 剩余容量
        size_t ncap = m_cur->size - npos;
        // 实际存储大小
        size_t bpos = 0;
        while (size > 0) {
            // 有足够容量
            if (ncap >= size) {
                memcpy((char *) buf + bpos, m_cur->ptr + npos, size);
                // 如果刚刚占完就换页
                if ((npos  + size) == m_cur->size) {
                    m_cur = m_cur->next;
                }
                m_position += size;
                size = 0;
                bpos += size;
            } else {
                // 如果大于剩余容量
                memcpy((char *) buf + bpos, m_cur->ptr + npos, ncap);
                m_position += ncap;
                size -= ncap;
                bpos += ncap;
                m_cur = m_cur->next;
                ncap = m_cur->size;
                npos = 0;
            }
        }
    };

    /**
     * 为tostring 准备 无副作用读
     * @param buf
     * @param size
     * @param position
     */
    void ByteArray::read(void *buf, size_t size, size_t position) const {
        if (size > getRemainSize()) {
            throw std::out_of_range("ByteArray::read out of range");
        }
        // 获取偏移量
        size_t npos = position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        size_t bpos = 0;
        Node *cur = m_cur;
        while (size > 0) {
            if (ncap >= size) {
                memcpy((char *) buf + bpos, cur->ptr + npos, size);
                if ((ncap + size) == cur->size) {
                    cur = cur->next;
                }
                bpos += size;
                size = 0;
                position += size;
            } else {
                memcpy((char *) buf + bpos, cur->ptr + npos, ncap);
                bpos += ncap;
                size -= ncap;
                position += ncap;
                cur = cur->next;
                ncap = cur->size;
            }
        }
    }

    bool ByteArray::writeToFile(const std::string &name) const {
        std::ofstream ofs;
        ofs.open(name, std::ios::trunc | std::ios::binary);
        if (!ofs) {
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG) << "open file " << name << " failed" <<
                                                              "error " << errno << " " << strerror(errno);
            return false;
        }

        // 获取剩余大小
        int64_t read_size = getRemainSize();
        // 获取偏移量
        int64_t pos = m_position;
        // 现在在使用的页
        Node *cur = m_cur;

        while (read_size > 0) {
            int diff = pos % m_baseSize;
            int64_t len = (read_size > (int64_t) m_baseSize ? m_baseSize : read_size) - diff;
            ofs.write(cur->ptr + diff, len);
            read_size -= len;
            pos += len;
            cur = cur->next;
        }
        return true;
    };

    bool ByteArray::readFromFile(const std::string &name) {
        std::ifstream ifs;
        ifs.open(name, std::ios::binary);
        if (!ifs) {
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG) << "open file " << name << " failed" <<
                                                              "error " << errno << " " << strerror(errno);
            return false;
        }
        std::shared_ptr<char> buf(new char[m_baseSize], [](char *ptr) { delete[] ptr; });
        // 读取文件类容到存储空间
        while (!ifs.eof()) {
            ifs.read(buf.get(), m_baseSize);
            write(buf.get(), ifs.gcount());
        }
        return true;
    };

    /**
     * 设置偏移
     * @param v
     */
    void ByteArray::setPosition(size_t v) {
        if (v > m_capacity) {
            throw std::out_of_range("set_position out of range");
        }
        m_position = v;
        if (m_position > m_size) {
            m_size = m_position;
        }
        m_cur = m_root;
        while (v > m_cur->size) {
            v -= m_cur->size;
            m_cur = m_cur->next;
        }
        if (v == m_cur->size) {
            m_cur = m_cur->next;
        }
    }

    void ByteArray::addCapacity(size_t size) {
        if (size == 0) {
            return;
        }
        // 获取剩余容量
        size_t old_capacity = getCapacity();
        if (old_capacity >= size) {
            // 修改的空间没有原来大
            return;
        }
        // > 多少容量
        size = size - old_capacity;
        // 计算需要扩容多少页
        int cont = size / m_baseSize + ((size_t) (size % m_baseSize) > old_capacity ? 1 : 0);
        // 找到最后一个节点
        Node *tmp = m_root;
        while (tmp->next) {
            tmp = tmp->next;
        }

        Node *first = nullptr;
        for (int i = 0; i < cont; i++) {
            tmp->next = new Node(m_baseSize);
            if (first == nullptr) {
                first = tmp->next;
            }
            tmp = tmp->next;
            m_capacity += m_baseSize;
        }
        if (old_capacity == 0) {
            // 剩余容量==0 代表需要到下一个节点
            m_cur = first;
        }
    }

    bool ByteArray::isLittleEndian() const {
        return m_endian != HH_BIG_ENDIAN;
    }

    void ByteArray::setLittleEndian(bool value) {
        m_endian = value ? HH_LITTLE_ENDIAN : HH_BIG_ENDIAN;
    }

    void ByteArray::writeFuint8(uint8_t value) {
        write(&value, sizeof(value));
    }

    std::string ByteArray::toString() const {
        std::string str;
        str.resize(getRemainSize());
        if (str.empty()) {
            // 没有可处理的数据
            return str;
        }
        read(&str[0], str.size(), m_position);
        return str;
    }

    std::string ByteArray::toHexString() const {
        std::string str = toString();
        std::stringstream ss;
        for (size_t i = 0; i < str.size(); i++) {
            if (i > 0 && i % 32 == 0) {
                ss << std::endl;
            }
            // 设置宽度为2    补0   输出为16进制   将字符串转uint  在变成int
            // uint8_t确保字符被视为无符号类型，避免在处理高ASCII或Unicode字符时出现意外的符号扩展。
            ss << std::setw(2) << std::setfill('0') << std::hex << (int) (uint8_t) str[i] << " ";
        }
        return ss.str();
    }

    uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len) const {
        len = len > getRemainSize() ? getRemainSize() : len;
        if (len == 0) {
            return 0;
        }
        uint64_t size = len;
        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        Node *cur = m_cur;
        struct iovec iov{};
        while (len > 0) {
            if (ncap >= len) {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;
                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }

    uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len, size_t position) const {
        len = len > getRemainSize() ? getRemainSize() : len;
        if (len == 0) {
            return 0;
        }
        // 算出读取偏移
        size_t npos = position % m_baseSize;
        uint64_t size = len;
        // 计算需要偏移页
        int cont = position / m_baseSize;
        Node *cur = m_root;
        while (cont > 0) {
            cur = cur->next;
            cont--;
        }
        uint64_t ncap = cur->size - npos;
        struct iovec iov{};
        while (len > 0) {
            if (ncap >= len) {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;
                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }

    uint64_t ByteArray::getWriteBuffers(std::vector<iovec> &buffers, uint64_t len) {
        if (len == 0) {
            return 0;
        }
        addCapacity(len);
        uint64_t size = len;
        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        Node *cur = m_cur;
        struct iovec iov{};
        while (len > 0) {
            if (ncap >= len) {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;
                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }
}