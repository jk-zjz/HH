#include "bytearray.h"
#include "log.h"
#include "macro.h"

static hh::Logger::ptr g_logger = HH_LOG_ROOT();

void test2() {
    std::vector<int8_t> vec;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(rand());
    }
    hh::ByteArray::ptr ba(new hh::ByteArray(3));
    for (auto &i: vec) {
        ba->writeFint8(i);
    }
    ba->setPosition(0);
    for (size_t i = 0; i < vec.size(); ++i) {
        int8_t v = ba->readFint8();
        HH_ASSERT(v == vec[i]);
    }
    HH_ASSERT(ba->getRemainSize() == 0);
}

void test() {
#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    hh::ByteArray::ptr ba(new hh::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        HH_ASSERT(v == vec[i]); \
    } \
    HH_ASSERT(ba->getRemainSize() == 0); \
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
}

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    hh::ByteArray::ptr ba(new hh::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        HH_ASSERT(v == vec[i]); \
    } \
    HH_ASSERT(ba->getRemainSize() == 0); \
    HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    ba->setPosition(0); \
    HH_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-123  .dat")); \
    hh::ByteArray::ptr ba2(new hh::ByteArray(base_len * 2)); \
    HH_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    ba2->setPosition(0); \
    HH_ASSERT(ba->toString() == ba2->toString()); \
    HH_ASSERT(ba->getPosition() == 0); \
    HH_ASSERT(ba2->getPosition() == 0); \
}
    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);

#undef XX
}

int main(int argc, char **argv) {
    //test();
    //test2();

    hh::ByteArray::ptr ba(new hh::ByteArray(4096));
    hh::ByteArray::ptr ba2(new hh::ByteArray(4096));
    std::vector<int32_t> vec;
    for (int32_t i = 0; i < 100; ++i) {
        // 通过未压缩方式存储
        ba->writeFint32(i);
        // 通过压缩方式存储 7bit 与 Zigzag
        ba2->writeInt32(i);
    }
// 设置偏移量到0
    ba->setPosition(0);
    ba2->setPosition(0);

// 读取数据
    for (int32_t i = 0; i < 100; ++i) {
        int32_t v = ba->readFint32();
        HH_LOG_FAT_INFO(g_logger, "v:= %d  i=%d", v, i)
    }
// 文件存储
    ba2->writeToFile("./test.txt");
    hh::ByteArray::ptr ba3(new hh::ByteArray(4096 * 2));
    // 读取文件
    ba3->readFromFile("./test.txt");
    // 因为写入了，所以要设置偏移量为0
    ba3->setPosition(0);
    if (ba3->toString() == ba2->toString()) {
        HH_LOG_INFO(g_logger, "ba3 == ba2");
    }

    return 0;
}