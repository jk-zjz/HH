//
// Created by 35148 on 2024/6/23.
//
#include "endian.h"
#include <istream>
#include <sstream>
#include "address.h"
namespace hh{
    int Address::getFamily() const {
        return getAddr()->sa_family;
    }

    std::string Address::toString() const {
        std::stringstream ss;
        insert(ss);
        return ss.str();
    }

    bool Address::operator<(const Address &rhs) const {
        // 获取比较长度
        socklen_t minlen = std::min(getAddrLen(),rhs.getAddrLen());
        // 字节比较
        int ret = memcmp(getAddr(),rhs.getAddr(),minlen);
        if(ret<0){
            // this < rhs
            return true;
        }else if(ret>0){
            // this > rhs
            return false;
        }else if(getAddrLen()<rhs.getAddrLen()){
            // this < rhs
            return true;
        }
        return false;
    }

    bool Address::operator==(const Address &rhs) const {
        return getAddrLen()==rhs.getAddrLen()&&
        memcmp(getAddr(),rhs.getAddr(),getAddrLen())==0;
    }

    bool Address::operator!=(const Address &rhs) const {
        return !(*this == rhs);
    }

    IPv4Address::IPv4Address(uint32_t address, uint32_t port) {
        memset(&m_addr,0,sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = byteswapOnLittleEndian(port);
        m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);
    }

    const sockaddr *IPv4Address::getAddr() const {
        return (sockaddr *)&m_addr;
    }

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
        return hh::IPAddress::ptr();
    }

    IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
        return hh::IPAddress::ptr();
    }

    IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
        return hh::IPAddress::ptr();
    }

    uint32_t IPv4Address::getPort() const {
        return byteswapOnLittleEndian(m_addr.sin_port);
    }

    void IPv4Address::setPort(uint32_t port) {
        m_addr.sin_port = byteswapOnLittleEndian(port);
    }

    IPv6Address::IPv6Address(const char *  address, uint32_t port) {
        memset(&m_addr,0,sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
        m_addr.sin6_port = byteswapOnLittleEndian(port);
        memcpy(&m_addr.sin6_addr,address,16);
    }

    const sockaddr *IPv6Address::getAddr() const {
        return (sockaddr *)&m_addr;
    }

    /**
     * 0001:db8:0120:0100:1200::ff00:8329
     * [1:db8:120:100:1200::ff00:8329]:8080
     */
    std::ostream &IPv6Address::insert(std::ostream &os) const {
        os << "[";
        uint16_t *addr = (uint16_t *)&m_addr.sin6_addr;
        bool use_zero = false;
        for(size_t i=0;i<8;++i){
            if(addr[i] == 0 && !use_zero){
                continue;
            }
            if(i && addr[i-1] == 0 && !use_zero){
                os << ":";
                use_zero = true;
            }
            os<<std::hex<<(int) byteswapOnLittleEndian(addr[i]);
        }
        if(!use_zero && addr[7] == 0){
            os << "::";
        }
        os << "]:" << byteswapOnLittleEndian(m_addr.sin6_port);
        return os;
    }

    IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
        return hh::IPAddress::ptr();
    }

    IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
        return hh::IPAddress::ptr();
    }

    IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
        return hh::IPAddress::ptr();
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
        memset(&m_addr,0,sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
    }

    UnixAddress::UnixAddress(const std::string &path) {
    }

    const sockaddr *UnixAddress::getAddr() const {
        return nullptr;
    }

    socklen_t UnixAddress::getAddrLen() const {
        return 0;
    }

    std::ostream &UnixAddress::insert(std::ostream &os) const {

    }

    UnixAddress::UnixAddress() {

    }

    const sockaddr *UnknownAddress::getAddr() const {
        return nullptr;
    }

    socklen_t UnknownAddress::getAddrLen() const {
        return 0;
    }

    std::ostream &UnknownAddress::insert(std::ostream &os) const {
        return <#initializer#>;
    }

    UnknownAddress::UnknownAddress(int family) {

    }
}
