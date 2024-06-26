//
// Created by 35148 on 2024/6/23.
//
#include <istream>
#include <memory>
#include <sstream>
#include "address.h"
#include "endian.h"
#include "log.h"

namespace hh {
    /**
     * 创建掩码
     * @tparam T 为 uint_32
     * @param bits 为 8
     * @return 0x00FFFFF
     * (1 << (4* 8 - 8)) - 1
     * (1<<24-1;
     * 1 0000 0000 0000 0000 0000 0000 -1;
     * 0 1111 1111 1111 1111 1111 1111
     * 0x00FFFFF
     */
    template<class T>
    static T creataMask(uint32_t bits) {
        return (1 << (sizeof(T) * 8 - bits)) - 1;
    }

    template<class T>
    static uint32_t countBits(T value){
        uint32_t ret = 0;
        for(;value;ret++){
            value &= value-1;
        }
        return ret;
    }

    static hh::Logger::ptr g_logger = HH_LOG_NAME("system");

/**
 * 获取地址族
 * @return 地址族
 */
    int Address::getFamily() const {
        return getAddr()->sa_family;
    }

/**
 * 获取地址字符串
 * @return 地址字符串
 */
    std::string Address::toString() const {
        std::stringstream ss;
        insert(ss);
        return ss.str();
    }

/**
 * 比较地址
 * @param rhs
 * @return
 */
    bool Address::operator<(const Address &rhs) const {
        // 获取比较长度
        socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
        // 字节比较
        int ret = memcmp(getAddr(), rhs.getAddr(), minlen);
        if (ret < 0) {
            // this < rhs
            return true;
        } else if (ret > 0) {
            // this > rhs
            return false;
        } else if (getAddrLen() < rhs.getAddrLen()) {
            // this < rhs
            return true;
        }
        return false;
    }

/**
 * 比较地址
 * @param rhs
 * @return
 */
    bool Address::operator==(const Address &rhs) const {
        return getAddrLen() == rhs.getAddrLen() &&
               memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
    }

/**
 * 不相等
 * @param rhs
 * @return
 */
    bool Address::operator!=(const Address &rhs) const {
        return !(*this == rhs);
    }

    Address::ptr Address::create(const sockaddr *addr, socklen_t len) {
        if(!addr){
            return nullptr;
        }
        Address::ptr result;
        switch (addr->sa_family) {
            case AF_INET:
                result.reset(new IPv4Address(*(sockaddr_in *) addr));
                break;
            case AF_INET6:
                result.reset(new IPv6Address(*(sockaddr_in6 *) addr));
                break;
            default:
                result.reset(new UnknownAddress(*(sockaddr *)addr));
                break;
        }
        return result;
    }


    /**
     * @brief 查找给定主机名对应的IP地址，并将结果存入输出向量中。
     *
     * 此函数执行DNS查询，将主机名解析为一个或多个IP地址。支持指定地址族、套接字类型和协议。
     *
     * @param result 输出参数，用于存储查找到的Address对象的智能指针集合。
     * @param host 要查询的主机名或域名。
     * @param family 地址族，如AF_INET（IPv4，默认）或AF_INET6（IPv6）。
     * @param type 套接字类型，默认为0，通常自动选择合适类型（如SOCK_STREAM）。
     * @param protocol IP协议，默认为0，通常自动匹配合适的协议（如IPPROTO_TCP）。
     *
     * @return 如果查找操作成功执行，则返回true；否则返回false。
     */
    bool Address::lookup(std::vector<Address::ptr> &result, const std::string &host, int family, int type, int protocol) {
        addrinfo hints, *results, *next;
        memset(&hints, 0, sizeof(addrinfo));
        hints.ai_family = family;
        hints.ai_socktype = type;
        hints.ai_protocol = protocol;

        std::string node;
        const char *service = nullptr;

        if (!host.empty() && host[0] == '[') {
            // 是否是IPv6地址
            const char *endipv6 = (const char *)memchr(host.c_str() + 1, ']', host.size() - 1);
            if (endipv6) {
                if (*(endipv6 + 1) == ':') {
                    service = endipv6 + 2;
                }
                node = host.substr(1, endipv6 - host.c_str() - 1);
            }
        } else {
            service = (const char *)memchr(host.c_str(), ':', host.size());
            if (service) {
                node = host.substr(0, service - host.c_str());
                service++;
            }
        }

        if (node.empty()) {
            node = host;
        }

        std::cout << "Resolving node: " << node << ", service: " << (service ? service : "null") << std::endl;

        int error = getaddrinfo(node.c_str(), service, &hints, &results);
        if (error) {
            std::cerr << "Address::lookup(" << host << ", " << family << ", " << type << ", " << protocol
                      << ") error=" << error << " errstr=" << gai_strerror(error) << std::endl;
            return false;
        }

        next = results;
        while (next) {
            result.push_back(create(next->ai_addr, (socklen_t)next->ai_addrlen));
            next = next->ai_next;
        }
        freeaddrinfo(results);
        return true;
    }
    /**
     * 根据给定的主机名、地址族、类型和协议，查找并返回第一个匹配的地址对象。
     *
     * @param host 要查找的主机名或IP地址。
     * @param family 地址族，如AF_INET用于IPv4，AF_INET6用于IPv6。
     * @param type 地址类型，如 SOCK_STREAM 用于TCP，SOCK_DGRAM 用于UDP。
     * @param protocol 协议，对应于地址类型，如TCP或UDP的协议号。
     * @return 如果找到匹配的地址，则返回一个指向地址对象的指针；否则返回nullptr。
     */
    Address::ptr Address::lookupAny(const std::string &host, int family, int type, int protocol) {
        std::vector<Address::ptr> result; // 用于存储查询结果的向量
        // 尝试查找匹配的地址，如果成功，结果将存储在result中
        if(lookup(result, host, family, type, protocol)){
            // 如果找到至少一个匹配的地址，返回第一个地址
            return result[0];
        }
        // 如果没有找到匹配的地址，返回nullptr
        return nullptr;
    }

    /**
     * 查找给定主机名、地址族、类型和协议下的任意IP地址，并将其转换为IPAddress对象返回。
     *
     * @param host 要查询的主机名或IP地址字符串。
     * @param family 指定地址族，例如AF_INET（IPv4）或AF_INET6（IPv6）。
     * @param type 地址对应的套接字类型，如SOCK_STREAM（TCP）或SOCK_DGRAM（UDP）。
     * @param protocol 指定的协议类型，如IPPROTO_TCP或IPPROTO_UDP。
     * @return 查找到的第一个IPAddress对象的智能指针，如果没有找到则返回nullptr。
     */
    std::shared_ptr<IPAddress> Address::lookupAnyIPAddress(const std::string& host,
                                                          int family,
                                                          int type,
                                                          int protocol){
        std::vector<Address::ptr> result; // 存储地址查询结果的容器
        // 执行地址查找操作
        if(lookup(result, host, family, type, protocol)){
            // 遍历所有查找到的地址对象
            for(auto &i:result){
                // 尝试将地址对象动态转换为IPAddress类型
                IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(i);
                // 如果转换成功，即找到了IPAddress对象，则返回之
                if(v){
                    return v;
                }
            }
        }
        // 如果没有找到合适的IPAddress对象，则返回nullptr
        return nullptr;
    }

    bool
    Address::GetInterfaceAddresses(std::multimap<std::string, std::pair<Address::ptr, uint32_t>> &result, int family) {
        struct ifaddrs *next, *results;
        // 获取接口地址列表
        if(getifaddrs(&results) != 0){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG) << "Address::GetInterfaceAddresses failed, errno="
                                                              << errno << " errstr=" << strerror(errno);
            return false;
        }
        try {
            // 循环遍历接口地址列表
            for(next = results; next; next = next->ifa_next){
                Address::ptr address;
                uint32_t prefix_len = 0;
                if(family != AF_UNSPEC && next->ifa_addr->sa_family != family){
                    continue;
                }
                switch (next->ifa_addr->sa_family) {
                    case AF_INET: {
                        address = create(next->ifa_addr, sizeof(sockaddr_in));
                        uint32_t netmask = ((sockaddr_in *)next->ifa_netmask)->sin_addr.s_addr;
                        prefix_len = countBits(netmask);
                        break;
                    }
                    case AF_INET6: {
                        HH_LOG_INFO(g_logger, "Address::GetInterfaceAddresses family=AF_INET6");
                        address = create(next->ifa_addr, sizeof(sockaddr_in6));
                        in6_addr& netmask_ = ((sockaddr_in6 *)next->ifa_netmask)->sin6_addr;
                        prefix_len = 0;
                        for (int i = 0; i < 16; ++i) {
                            prefix_len += countBits(netmask_.s6_addr[i]);
                        }
                        break;
                    }
                    default:
                        break;
                }
                if(address){
                    result.insert(std::make_pair(next->ifa_name, std::make_pair(address, prefix_len)));
                }
            }
        }catch(...){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<< "Address::GetInterfaceAddresses except";
            freeifaddrs(results);
            return false;
        }
        freeifaddrs(results);
        return true;
    }

    bool
    Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>> &result, const std::string &iface,
                                   int family) {
        // 如果没有指定网卡名称或者为*返回所有
        if(iface.empty() || iface=="*"){
            // 是ipv4 或者 不规范的地址
            if(family==AF_INET || family == AF_UNSPEC){
                result.push_back(std::make_pair(Address::ptr(new IPv4Address()), 0));
            }
            // 是ipv6 或者 不规范的地址
            if(family==AF_INET6 || family == AF_UNSPEC){
                result.push_back(std::make_pair(Address::ptr(new IPv6Address()), 0));
            }
            return true;
        }
        // 如果有网卡名称
        std::multimap<std::string, std::pair<Address::ptr, uint32_t>> results;
        if(!GetInterfaceAddresses(results, family)){
            return false;
        }
        // 查询范围  [] 的相同名称网卡
        auto its = results.equal_range(iface);
        for(; its.first != its.second; ++its.first){
            // 添加到结果中
            result.push_back(its.first->second);
        }
        return true;
    }

    /**
     * 构造函数
     * @param address
     * @param port
     */
    IPv4Address::IPv4Address(uint32_t address, uint32_t port) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = byteswapOnLittleEndian(port);
        m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);
    }

/**
 * 获取地址sockaddr
 * @return
 */
    const sockaddr *IPv4Address::getAddr() const {
        return (sockaddr *) &m_addr;
    }

/**
 * 获取地址长度
 * @return
 */
    socklen_t IPv4Address::getAddrLen() const {
        return sizeof(m_addr);
    }

// 0xC0A80101 是一个32位的16进制数
    std::ostream &IPv4Address::insert(std::ostream &os) const {
        uint32_t addr_net = byteswapOnLittleEndian(m_addr.sin_addr.s_addr);
        // 通过每一次比较8为二进制吧32位拆分为点分十进制
        os << ((addr_net >> 24) & 0xff);
        os << "." << ((addr_net >> 16) & 0xff);
        os << "." << ((addr_net >> 8) & 0xff);
        os << "." << (addr_net & 0xff);
        os << ":" << byteswapOnLittleEndian(m_addr.sin_port);
        return os;
    }

    IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
        if (prefix_len > 32) {
            return nullptr;
        }
        sockaddr_in broadcast_addr(m_addr);
        broadcast_addr.sin_addr.s_addr |=
                byteswapOnLittleEndian(creataMask<uint32_t>(prefix_len));
        return std::make_shared<IPv4Address>(broadcast_addr);
    }

    IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
        if (prefix_len > 32) {
            return nullptr;
        }
        sockaddr_in broadcast_addr(m_addr);
        broadcast_addr.sin_addr.s_addr &=
                byteswapOnLittleEndian(creataMask<uint32_t>(prefix_len));
        return std::make_shared<IPv4Address>(broadcast_addr);
    }

    IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
        sockaddr_in subnet_mask;
        memset(&subnet_mask, 0, sizeof(subnet_mask));
        subnet_mask.sin_family = AF_INET;
        subnet_mask.sin_addr.s_addr = ~byteswapOnLittleEndian(creataMask<uint32_t>(prefix_len));
        return std::make_shared<IPv4Address>(subnet_mask);
    }

/**
 * 获取端口
 * @return
 */
    uint32_t IPv4Address::getPort() const {
        return byteswapOnLittleEndian(m_addr.sin_port);
    }

/**
 * 设置端口
 * @param port
 */
    void IPv4Address::setPort(uint32_t port) {
        m_addr.sin_port = byteswapOnLittleEndian(port);
    }

    IPv4Address::IPv4Address(const sockaddr_in &address) {
        m_addr = address;
    }

    IPv4Address::ptr IPv4Address::Create(const std::string &address, uint32_t port) {
        IPv4Address::ptr ret(new IPv4Address);
        ret->m_addr.sin_port = byteswapOnLittleEndian(port);
        int result = inet_pton(AF_INET, address.c_str(), &ret->m_addr.sin_addr);
        if (result <= 0) {
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG) << "inet_pton fail" << std::endl;
            return nullptr;
        }
        return ret;
    }

/**
 * 获取地址sockaddr
 * @return
 */
    const sockaddr *IPv6Address::getAddr() const {
        return (sockaddr *) &m_addr;
    }

/**
 * 0001:db8:0120:0100:1200::ff00:8329
 * [1:db8:120:100:1200::ff00:8329]:8080
 */
    std::ostream &IPv6Address::insert(std::ostream &os) const {
        os << "[";
        uint16_t *addr = (uint16_t *) &m_addr.sin6_addr;
        bool use_zero = false;
        for (size_t i = 0; i < 8; ++i) {
            if (addr[i] == 0 && !use_zero) {
                continue;
            }
            if (i && addr[i - 1] == 0 && !use_zero) {
                os << ":";
                use_zero = true;
            }
            os << std::hex << (int) byteswapOnLittleEndian(addr[i]);
        }
        if (!use_zero && addr[7] == 0) {
            os << "::";
        }
        os << "]:" << byteswapOnLittleEndian(m_addr.sin6_port);
        return os;
    }

    IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
        sockaddr_in6 broadcast_addr(m_addr);
        broadcast_addr.sin6_addr.s6_addr[prefix_len / 8] |=
                creataMask<uint8_t>(prefix_len % 8);
        for (int i = prefix_len / 8; i < 16; ++i) {
            broadcast_addr.sin6_addr.s6_addr[i] = 0xff;
        }
        return std::make_shared<IPv6Address>(broadcast_addr);
    }

    IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
        sockaddr_in6 network_addr(m_addr);
        network_addr.sin6_addr.s6_addr[prefix_len / 8] &=
                creataMask<uint8_t>(prefix_len % 8);
        for (int i = prefix_len / 8; i < 16; ++i) {
            network_addr.sin6_addr.s6_addr[i] = 0x00;
        }
        return std::make_shared<IPv6Address>(network_addr);
    }

    IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
        sockaddr_in6 subnet_mask;
        memset(&subnet_mask, 0, sizeof(subnet_mask));
        subnet_mask.sin6_family = AF_INET6;
        subnet_mask.sin6_addr.s6_addr[prefix_len / 8] =
                creataMask<uint8_t>(prefix_len % 8);
        for (int i = prefix_len / 8; i < 16; ++i) {
            subnet_mask.sin6_addr.s6_addr[i] = 0xff;
        }
        return std::make_shared<IPv6Address>(subnet_mask);
    }

    uint32_t IPv6Address::getPort() const {
        return byteswapOnLittleEndian(m_addr.sin6_port);
    }

    void IPv6Address::setPort(uint32_t port) {
        m_addr.sin6_port = byteswapOnLittleEndian(port);
    }

    socklen_t IPv6Address::getAddrLen() const {
        return 0;
    }

    IPv6Address::IPv6Address() {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
    }

    IPv6Address::IPv6Address(const sockaddr_in6 &address) {
        m_addr = address;
    }

    /**
     * 构造函数
     * @param address
     * @param port
     */
    IPv6Address::IPv6Address(const uint8_t *address, uint32_t port) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
        m_addr.sin6_port = byteswapOnLittleEndian(port);
        memcpy(&m_addr.sin6_addr, address, 16);
    }

    IPv6Address::ptr IPv6Address::Create(const char *address, uint32_t port) {
        IPv6Address::ptr ret(new IPv6Address);
        ret->m_addr.sin6_port = byteswapOnLittleEndian(port);
        int result = inet_pton(AF_INET6, address, &ret->m_addr.sin6_addr);
        if (result <= 0) {
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG) << "inet_pton fail" << std::endl;
            return nullptr;
        }
        return ret;
    }

    UnixAddress::UnixAddress(const std::string &path) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sun_family = AF_UNIX;
        // 计算字符串大小
        m_length = path.size() + 1;
        // 如果字符串不空，并且第一个字符为0，就不需要+1
        if (!path.empty() && path[0] == '\0') {
            m_length--;
        }
        // 如果字符串长度大于sun_path的长度，则抛出异常
        if (m_length > sizeof(m_addr.sun_path)) {
            throw std::invalid_argument("path too long");
        }
        memcpy(m_addr.sun_path, path.c_str(), m_length);
        // 调整m_length的值，增加sockaddr_un结构体中sun_path成员的偏移量，
        // 以便正确表示整个结构体的大小，这对于后续的套接字操作（如绑定或连接）是必要的。
        m_length += offsetof(sockaddr_un, sun_path);
    }

    const sockaddr *UnixAddress::getAddr() const {
        return (sockaddr *) &m_addr;
    }

    socklen_t UnixAddress::getAddrLen() const {
        return sizeof(m_addr);
    }

    std::ostream &UnixAddress::insert(std::ostream &os) const {
        if (m_length > offsetof(sockaddr_un, sun_path)
            && m_addr.sun_path[0] == '\0') {
            // 如果m_length>偏移量，代表需要截取，从 m_addr.sun_path读-长度-偏移量
            return os << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
        }
        return os << m_addr.sun_path;
    }

    UnixAddress::UnixAddress() {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sun_family = AF_UNIX;
    }

    const sockaddr *UnknownAddress::getAddr() const {
        return (sockaddr *) &m_addr;
    }

    socklen_t UnknownAddress::getAddrLen() const {
        return sizeof(m_addr);
    }

    std::ostream &UnknownAddress::insert(std::ostream &os) const {
        os << "[UnknownAddress family=" << m_addr.sa_family;
        return os << "]";
    }

    UnknownAddress::UnknownAddress(int family) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sa_family = family;
    }

    UnknownAddress::UnknownAddress(const sockaddr &addr) {
        m_addr = addr;
    }

    IPAddress::ptr IPAddress::Create(const char *address, uint16_t port) {
        addrinfo hint, *results;
        memset(&hint, 0, sizeof(hint));
        hint.ai_flags = AI_NUMERICHOST;
        hint.ai_family = AF_UNSPEC;
        int ret = getaddrinfo(address, nullptr, &hint, &results);
        if(ret != 0){
            HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::DEBUG) << "getaddrinfo(" << address << ") error=" << ret << " errno=" << errno << " errstr=" << strerror(errno) << std::endl;
            return nullptr;
        }
        try{
            IPAddress::ptr result =std::dynamic_pointer_cast<IPAddress>(
                    Address::create(results->ai_addr, (socklen_t)results->ai_addrlen));
            if(result){
                result->setPort(port);
            }
            freeaddrinfo(results);
            return result;
        }
        catch(...){
            // 释放内存
            freeaddrinfo(results);
            return nullptr;
        }
    }

}
