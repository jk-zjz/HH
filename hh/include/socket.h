//
// Created by 35148 on 2024/6/27.
//

#ifndef HH_SOCKET_H
#define HH_SOCKET_H
#include "address.h"
#include <memory>
#include "Noncopyable.h"
namespace hh {
    class Socket : public Noncopyable, std::enable_shared_from_this<Socket> {
    public:
        // 智能指针
        typedef std::shared_ptr<Socket> ptr;
        typedef std::weak_ptr<Socket> weak_ptr;
        // 构造函数 协议族，套接字类型，协议
        Socket(int family, int type, int protocol = 0);
        ~Socket();

        // 获取当前设置的发送数据超时时间（单位：毫秒）
        int64_t getSendTimeout();
        // 设置发送数据的超时时间，单位为毫秒
        void setSendTimeout(int64_t v);

        // 获取当前设置的接收数据超时时间（单位：毫秒）
        int64_t getRecvTimeout();
        // 设置接收数据的超时时间，单位为毫秒
        void setRecvTimeout(int64_t v);

        /**
         * 查询套接字的指定选项值
         * @param level 选项所在的协议层次，如SOL_SOCKET或IPPROTO_TCP
         * @param option 要查询的具体选项标识
         * @param result 输出缓冲区，用于存放查询到的选项值
         * @param len 输入时为result缓冲区的大小，输出时可能更新为实际返回值的大小
         * @return 成功获取选项值返回true，否则返回false
         */
        bool getOption(int level, int option, void *result, size_t *len);
        /**
         * 模板函数，简化获取选项的过程，自动处理结果类型的大小
         * @tparam T 选项值的类型
         * @param level 选项层次
         * @param option 选项标识要获取的特定选项，如SO_RCVTIMEO（接收超时）或SO_REUSEADDR（地址复用）
         * @param result 存储查询结果的变量引用
         * @return 操作成功返回true，否则返回false
         */
        template<class T>
        bool getOption(int level, int option, T &result){
            size_t len = sizeof(T);
            return getOption(level, option, &result, &len);
        }

        /**
         * 设置套接字的指定选项值
         * @param level 选项所在的协议层次
         * @param option 要设置的选项标识
         * @param result 要设置的新选项值
         * @param len 选项值的字节大小
         * @return 成功设置选项返回true，否则返回false
         */
        bool setOption(int level, int option, const void *result, size_t len);
        /**
         * 模板函数，简化设置选项的过程，自动计算选项值的大小
         * @tparam T 选项值的类型
         * @param level 选项层次
         * @param option 选项标识要获取的特定选项，如SO_RCVTIMEO（接收超时）或SO_REUSEADDR（地址复用）
         * @param result 要设置的选项值的引用
         * @return 操作成功返回true，否则返回false
         */
        template<class T>
        bool setOption(int level, int option, const T &result){
            return setOption(level, option, &result, sizeof(T));
        }
        // 提取链接
        Socket::ptr accept();
        // 绑定
        bool bind(const Address::ptr &address);
        // 建立连接
        bool connect(const Address::ptr &address, int timeout_ms = -1);
        //  监听
        bool listen(int backlog = SOMAXCONN);
        // 关闭
        bool close();
        // 通过句柄创建套接字
        bool init(int sock);

        /**
         * iovec结构体，iovec结构体描述了多个连续内存块
         * 每个内存块由iov_base和iov_len组成
         * iov_base指向内存块的起始地址，iov_len表示内存块的长度。
         */
        int send(const void *buffer, size_t length, int flags = 0);
        int send(const iovec *buffers, size_t length, int flags = 0);
        /**
         * 向指定地址发送缓冲区数据
         * @param buffer 包含待发送数据的缓冲区指针
         * @param length 缓冲区中待发送数据的字节数
         * @param to 目标地址的智能指针
         * @param flags 可选的发送标志
         * @return 实际发送的字节数，若发生错误则返回-1
         */
        int sendTo(const void *buffer, size_t length,const Address::ptr to,int flags = 0);
        /**
         * 向指定地址利用分散/聚集I/O发送数据
         * @param buffers 指向iovec结构数组的指针，描述多段独立数据缓冲区
         * @param length io矢量的数量
         * @param to 目标地址的智能指针
         * @param flags 可选的发送标志
         * @return 实际发送的总字节数，若发生错误则返回-1
         */
        int sendTo(const iovec *buffer, size_t length,const Address::ptr to,int flags = 0);

        int recv(void *buffer, size_t length, int flags = 0);
        int recv(iovec *buffer, size_t length, int flags = 0);
        /**
         * 从套接字接收数据，并获取数据来源地址
         * @param buffer 存储接收到数据的缓冲区指针
         * @param length 缓冲区大小
         * @param from 传出参数，用于接收数据发送方的地址信息
         * @param flags 可选接收标志
         * @return 实际接收到的字节数，若发生错误或连接关闭则返回-1
         */
        int recvFrom(void *buffer, size_t length, Address::ptr &from, int flags = 0);

        /**
         * 利用分散/聚集I/O接收数据，并获取数据来源地址
         * @param buffer 指向iovec结构数组的指针，描述接收数据的目标缓冲区
         * @param length io矢量的数量
         * @param from 传出参数，接收数据发送方的地址信息
         * @param flags 可选接收标志
         * @return 实际接收到的总字节数，若发生错误或连接关闭则返回-1
         */
        int recvFrom(iovec *buffer, size_t length, Address::ptr &from, int flags = 0);

        // 获本地取套接字地址
        Address::ptr getLocalAddress();
        // 获取对端地址
        Address::ptr getRemoteAddress();

        bool isConnected() const{return m_isConnected;};
        int getFamily() const{return m_family;};
        int getSock() const{return m_sock;}
        int getType() const{return m_type;}
        int getProtocol() const{return m_protocol;}
        bool isValid() const;
        int getError();
        std::ostream &dump(std::ostream &os) const;

        /**
         * 取消事件
         */
        bool cancelRead();
        bool cancelWrite();
        bool cancelAccept();
        bool cancelAll();
    private:
        // 初始化套接字
        void initSock();
        // 创建套接字
        void newSock();
    private:
    // 套接字描述符，用于标识和操作网络连接
    int m_sock;

    // 地址家族，如AF_INET(IPv4)或AF_INET6(IPv6)，定义了套接字可处理的地址类型
    int m_family;

    // 套接字类型，如SOCK_STREAM(TCP)或SOCK_DGRAM(UDP)，决定了数据传输模式和协议特性
    int m_type;

    // 使用的具体协议，如IPPROTO_TCP或IPPROTO_UDP，默认根据m_family和m_type自动选择
    int m_protocol;

    // 连接状态标志，true表示套接字已成功建立连接，false表示未连接或连接断开
    bool m_isConnected;

    // 指向本地绑定地址的智能指针，包含本端的IP地址和端口号信息
    hh::Address::ptr m_localAddress;

    // 指向对端地址的智能指针，对于客户端是服务端地址，对于服务端是客户端的地址和端口信息
    hh::Address::ptr m_remoteAddress;

    };
}
#endif //HH_SOCKET_H
