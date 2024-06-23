//
// Created by 35148 on 2024/6/23.
//

#ifndef HH_ADDRESS_H
#define HH_ADDRESS_H
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
namespace hh{
    class Address{
    public:
        // 使用std::shared_ptr智能指针来管理Address对象的生命周期
        typedef std::shared_ptr<Address> ptr;

        // 虚析构函数，确保通过基类指针删除派生类对象时能正确调用派生类的析构函数
        virtual ~Address(){}

        // 获取地址族（如AF_INET for IPv4, AF_INET6 for IPv6）
        int getFamily() const;

        // 返回指向sockaddr结构体的指针，用于获取地址信息，纯虚函数需在派生类中实现
        virtual const sockaddr* getAddr() const = 0;

        // 返回sockaddr结构体的大小，用于如bind、connect等函数中的长度参数，纯虚函数需在派生类中实现
        virtual socklen_t getAddrLen() const = 0;

        // 将Address对象插入到输出流中，以便打印或记录，纯虚函数需在派生类中实现
        virtual std::ostream &insert(std::ostream& os) const = 0;

        // 生成Address对象的字符串表示形式
        std::string toString() const;

        // 比较当前对象是否小于rhs对象，用于排序等操作
        virtual bool operator<(const Address& rhs) const;

        // 判断当前对象是否与rhs对象相等
        virtual bool operator==(const Address& rhs) const;

        // 判断当前对象是否与rhs对象不相等，通常由==运算符自动推导实现
        virtual bool operator!=(const Address& rhs) const;

    };
    class IPAddress : public Address{
    public:
        // 使用std::shared_ptr智能指针来管理IPAddress对象的生命周期，定义为IPAddress::ptr别名
        typedef std::shared_ptr<IPAddress> ptr;

        // 根据给定的前缀长度计算并返回广播地址，纯虚函数需在派生类中实现
        virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;

        // 根据给定的前缀长度计算并返回网络地址，纯虚函数需在派生类中实现
        virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;

        // 根据给定的前缀长度计算并返回子网掩码，纯虚函数需在派生类中实现
        virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

        // 获取IP地址关联的端口号
        virtual uint32_t getPort() const = 0;

        // 设置IP地址关联的端口号
        virtual void setPort(uint32_t port) = 0;
    };
    class IPv4Address : public IPAddress{
    public:
        typedef std::shared_ptr<IPv4Address> ptr;
        IPv4Address(uint32_t address = INADDR_ANY, uint32_t port = 0);

        // 返回指向存储IPv4地址信息的sockaddr结构体的指针，实现基类的getAddr()函数
        const sockaddr* getAddr() const override;

        // 返回sockaddr结构体的大小，实现基类的getAddrLen()函数
        socklen_t getAddrLen() const override;

        // 将IPv4Address对象的信息插入到输出流中，实现基类的insert()函数
        std::ostream &insert(std::ostream& os) const override;

        // 计算并返回广播地址，实现基类的broadcastAddress()函数
        IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;

        // 计算并返回网络地址，实现基类的networkAddress()函数
        IPAddress::ptr networkAddress(uint32_t prefix_len) override;

        // 计算并返回子网掩码，实现基类的subnetMask()函数
        IPAddress::ptr subnetMask(uint32_t prefix_len) override;

        // 获取与IPv4地址关联的端口号
        uint32_t getPort() const override;

        // 设置与IPv4地址关联的端口号
        void setPort(uint32_t port) override;
    private:
        sockaddr_in m_addr;
    };
    class IPv6Address : public IPAddress{
    public:
        typedef std::shared_ptr<IPv6Address> ptr;
        IPv6Address();
        IPv6Address( const char * address, uint32_t port = 0);

        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;
        std::ostream &insert(std::ostream& os) const override;
        IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
        IPAddress::ptr networkAddress(uint32_t prefix_len) override;
        IPAddress::ptr subnetMask(uint32_t prefix_len) override;
        uint32_t getPort() const override;
        void setPort(uint32_t port) override;
    private:
        sockaddr_in6 m_addr;
    };
    class UnixAddress : public Address{
    public:
        typedef std::shared_ptr<UnixAddress> ptr;
        UnixAddress(const std::string& path);
        UnixAddress();

        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;
        std::ostream &insert(std::ostream& os) const override;
    private:
        struct sockaddr_un m_addr;
        socklen_t m_length;
    };
    class UnknownAddress : public Address{
    public:
        typedef std::shared_ptr<UnknownAddress> ptr;
        UnknownAddress(int family);
        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;
        std::ostream &insert(std::ostream& os) const override;
    private:
        struct sockaddr m_addr;
    };

}
#endif //HH_ADDRESS_H
