//
// Created by 35148 on 2024/6/27.
//
#include "socket.h"
#include "fd_manager.h"
#include "log.h"
#include "macro.h"
#include "iomanage.h"
#include <netinet/tcp.h>
namespace hh{
static  hh::Logger::ptr g_logger = HH_LOG_NAME("system");
    Socket::Socket(int family, int type, int protocol):
    m_sock(-1),
    m_family(family),
    m_type(type),
    m_protocol(protocol),
    m_isConnected(false){
    }

    Socket::~Socket() {
        close();
    }

    int64_t Socket::getSendTimeout() {
        // 获取发送超时时间
        hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(m_sock);
        if(ctx) {
            return ctx->getTimeout(SO_SNDTIMEO);
        }
        return -1;
    }

    void Socket::setSendTimeout(int64_t v) {
        // 设置发送超时时间
        struct timeval tv;
        tv.tv_sec = (int) v / 1000;
        tv.tv_usec = (int) v % 1000 * 1000;
        setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
    }

    int64_t Socket::getRecvTimeout() {
        // 获取接收超时时间
        hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(m_sock);
        if(ctx) {
            return ctx->getTimeout(SO_RCVTIMEO);
        }
        return -1;
    }

    void Socket::setRecvTimeout(int64_t v) {
        // 设置接收超时时间
        struct timeval tv;
        tv.tv_sec = (int) v / 1000;
        tv.tv_usec = (int) v % 1000 * 1000;
        // SOL_SOCKET 设置为套接字，SO_RCVTIMEO 设置为接收超时
        setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
    }

    bool Socket::getOption(int level, int option, void *result, size_t *len) {
        // 获取套接字信息选项
        int rt = getsockopt(m_sock, level, option, result, (socklen_t *) len);
        if(rt){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"getsockopt error,errno="<<errno<<",error="<<strerror(errno);
            return false;
        }
        return true;
    }

    bool Socket::setOption(int level, int option, const void *result, size_t len) {
        // 设置套接字信息选项
        int rt = setsockopt(m_sock, level, option, result, (socklen_t) len);
        if(rt){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"setsockopt error,errno="<<errno<<",error="<<strerror(errno);
            return false;
        }
        return true;
    }

    Socket::ptr Socket::accept() {
        // 创建套接字
        Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
        // 提取连接
        int newsock = ::accept(m_sock, nullptr, nullptr);
        if(newsock == -1) {
            // 提取失败
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"accept error,errno="<<errno<<",error="<<strerror(errno);
            return nullptr;
        }
        // 提取成功初始化套接字，行套接字fd
        if(sock->init(newsock)) {
            return sock;
        }
        return nullptr;
    }

    bool Socket::bind(const Address::ptr &address) {
        if(!isValid()){
            // socket 无效
            // 创建socket
            newSock();
            if(HH_UNLIKELY(!isValid())){
                // 还是无效
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"newSock error,errno="
                <<errno<<",error="<<strerror(errno);
                close();
                return false;
            }
        }
        // 存储的和传入地址族不匹配
        if(HH_UNLIKELY(address->getFamily() != m_family)){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"address family error"
            <<address->getFamily()
            <<" "<<m_family
            <<address->toString();
            close();
            return false;
        }
        // 绑定失败不关闭文件描述符
        if(::bind(m_sock, address->getAddr(), address->getAddrLen())){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"bind error,errno="
            <<errno<<",error="<<strerror(errno);
            return false;
        };
        getLocalAddress();
        return true;
    }

    bool Socket::connect(const Address::ptr &address, uint64_t timeout_ms) {
        if(!isValid()){
            // socket 无效
            // 创建socket
            newSock();
            if(HH_UNLIKELY(!isValid())){
                // 还是无效
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"newSock error,errno="
                                                                <<errno<<",error="<<strerror(errno);
                close();
                return false;
            }
        }
        // 存储的和传入地址族不匹配
        if(HH_UNLIKELY(address->getFamily() != m_family)){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"address family error"
                                                            <<address->getFamily()
                                                            <<" "<<m_family
                                                            <<address->toString();
            close();
            return false;
        }
        if(timeout_ms == (uint64_t)-1){
            // 没有超时时间
            if(::connect(m_sock, address->getAddr(), address->getAddrLen()) != 0){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"connect error,errno="
                                                                <<errno<<",error="<<strerror(errno);
                close();
                return false;
            }
        }else{
            if(::connect_with_timeout(m_sock, address->getAddr(), address->getAddrLen(), timeout_ms)){
                HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"connect error,errno="
                                                                <<errno<<",error="<<strerror(errno);
                close();
                return false;
            }
        }
        getLocalAddress();
        getRemoteAddress();
        m_isConnected = true;
        return true;
    }

    bool Socket::listen(int backlog) {
        if(!isValid()){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"listen error,errno="
                                                            <<errno<<",error="<<strerror(errno);
            return false;
        }
        if(::listen(m_sock, backlog)){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"listen error,errno="
                                                            <<errno<<",error="<<strerror(errno);
            return false;
        }
        return true;
    }

    bool Socket::close() {
        if(!m_isConnected && !isValid()){
            return true;
        }
        if(HH_LIKELY(::close(m_sock) == 0)){
            m_sock = -1;
            m_isConnected = false;
            return true;
        }
        return false;
    }

    bool Socket::init(int sock) {
        // 从管理fd中获取socket信息
        hh::FdCtx::ptr ctx = hh::FdMgr::GetInstance()->get(sock);
        // 判断是否是socket && 未关闭
        if(ctx && ctx->isSocket() && !ctx->isClose()){
            m_sock = sock;
            m_isConnected = true;
            // 初始化套接字内部，获取当前地址，远端地址
            initSock();
            getLocalAddress();
            getRemoteAddress();
            return true;
        }
        return false;
    }

    int Socket::send(const void *buffer, size_t length, int flags) {
        if(m_isConnected){
            return ::send(m_sock, buffer, length, flags);
        }
        return -1;
    }

    int Socket::send(const iovec *buffers, size_t length, int flags) {
        if(m_isConnected){
            msghdr msg;
            memset(&msg, 0, sizeof(msg));
            msg.msg_iov = (iovec*)buffers;
            msg.msg_iovlen = length;
            return ::sendmsg(m_sock, &msg, flags);
        }
        return -1;
    }

    int Socket::sendTo(const void *buffer, size_t length, const Address::ptr to, int flags) {
        return ::sendto(m_sock, (msghdr*)buffer, length, flags, to->getAddr(), to->getAddrLen());
    }

    int Socket::sendTo(const iovec *buffer, size_t length, const Address::ptr to, int flags) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffer;
        msg.msg_iovlen = length;
        msg.msg_name = to->getAddr();
        msg.msg_namelen = to->getAddrLen();
        return ::sendmsg(m_sock, &msg, flags);
    }

    int Socket::recv(void *buffer, size_t length, int flags) {
        if(m_isConnected){
            return ::recv(m_sock, buffer, length, flags);
        }
        return -1;
    }

    int Socket::recv(iovec *buffer, size_t length, int flags) {
        if(m_isConnected){
            msghdr msg;
            memset(&msg, 0, sizeof(msg));
            msg.msg_iov = (iovec*)buffer;
            msg.msg_iovlen = length;
            return ::recvmsg(m_sock, &msg, flags);
        }
        return -1;
    }

    int Socket::recvFrom(void *buffer, size_t length, Address::ptr &from, int flags) {
        socklen_t fromlen = from->getAddrLen();
        return ::recvfrom(m_sock, buffer, length, flags, from->getAddr(), &fromlen);
    }

    int Socket::recvFrom(iovec *buffer, size_t length, Address::ptr &from, int flags) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffer;
        msg.msg_iovlen = length;
        msg.msg_name = from->getAddr();
        msg.msg_namelen = from->getAddrLen();
        return ::recvmsg(m_sock, &msg, flags);
        return 0;
    }

    Address::ptr Socket::getLocalAddress() {
        if(m_localAddress){
            return m_localAddress;
        }
        Address::ptr ret;
        switch(m_family){
            case AF_INET:
                ret.reset(new IPv4Address());
                break;
            case AF_INET6:
                ret.reset(new IPv6Address());
                break;
            case AF_UNIX:
                ret.reset(new UnixAddress());
            default:
                ret.reset(new UnknownAddress(m_family));
                break;
        }
        socklen_t len = ret->getAddrLen();
        if(getsockname(m_sock, ret->getAddr(), &len)==-1){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"getLocalAddress error,errno="
                                                            <<errno<<",error="<<strerror(errno);
            return Address::ptr(new UnknownAddress(m_family));
        }
        if(m_family==AF_UNIX){
            UnixAddress::ptr address = std::dynamic_pointer_cast<UnixAddress>(ret);
            if(address){
                address->setAddrLen(len);
            }
        }
        m_localAddress=ret;
        return m_localAddress;
    }

    Address::ptr Socket::getRemoteAddress() {
        if(m_remoteAddress){
            return m_remoteAddress;
        }
        Address::ptr ret;
        switch(m_family){
            case AF_INET:
                ret.reset(new IPv4Address());
                break;
            case AF_INET6:
                ret.reset(new IPv6Address());
                break;
            case AF_UNIX:
                ret.reset(new UnixAddress());
                break;
            default:
                ret.reset(new UnknownAddress(m_family));
                break;
        }
        socklen_t len = ret->getAddrLen();
        if(getpeername(m_sock, ret->getAddr(), &len)==-1){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"getRemoteAddress error,errno="
                                                            <<errno<<",error="<<strerror(errno);
            return Address::ptr(new UnknownAddress(m_family));
        }
        if(m_family==AF_UNIX){
            UnixAddress::ptr address = std::dynamic_pointer_cast<UnixAddress>(ret);
            if(address){
                address->setAddrLen(len);
            }
        }
        m_remoteAddress=ret;
        return m_remoteAddress;
    }

    bool Socket::isValid() const {
        return m_sock != -1;
    }

    int Socket::getError() {
        int error = 0;
        size_t len = sizeof(error);
        if(!getOption(SOL_SOCKET, SO_ERROR, &error, &len)){
            return -1;
        }
        return error;
    }

    std::ostream &Socket::dump(std::ostream &os) const {
        os<<"[ socket: "<<m_sock
        <<" family="<<m_family
        <<" type="<<m_type
        <<" protocol="<<m_protocol
        <<" isConnected="<<m_isConnected
        <<" localAddress="<<(m_localAddress ? m_localAddress->toString() : "")
        <<" remoteAddress="<<(m_remoteAddress ? m_remoteAddress->toString() : "")
        <<"]";
        return os;
    }
    std::ostream &operator<<(std::ostream &os, const Socket &sock){
        return sock.dump(os);
    }
    // 取消事件
    bool Socket::cancelRead() {
        return hh::IOManager::GetThis()->cancelEvent(m_sock, hh::IOManager::READ);
    }

    bool Socket::cancelWrite() {
        return hh::IOManager::GetThis()->cancelEvent(m_sock, hh::IOManager::WRITE);
    }

    bool Socket::cancelAccept() {
        return hh::IOManager::GetThis()->cancelEvent(m_sock, hh::IOManager::READ);
    }

    bool Socket::cancelAll() {
        return hh::IOManager::GetThis()->cancelAll(m_sock);
    }

    void Socket::initSock() {
        int val =1;
        // 设置端口复用
        setOption(SOL_SOCKET, SO_REUSEADDR, val);
        // 判断是否为流式套接字，还有一种是UDP的
        if(m_type == SOCK_STREAM){
            //这行代码的功能是根据val的值来决定是否在TCP连接上禁用Nagle算法，以优化数据传输的延迟特性
            setOption(IPPROTO_TCP, TCP_NODELAY, val);
        }
    }

    void Socket::newSock() {
        // 创建套接字fd,并且初始化套接字内部
        m_sock = socket(m_family, m_type, m_protocol);
        if(HH_LIKELY(m_sock != -1)){
            initSock();
        }else{
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::DEBUG)<<"socket error,errno="<<errno<<",error="<<strerror(errno);
        }
    }
    Socket::ptr Socket::CreateTCP(Address::ptr address){
        Socket::ptr sock(new Socket(address->getFamily(),TCP,0));
        return sock;
    }

    Socket::ptr Socket::CreateUDP(Address::ptr address) {
        Socket::ptr sock(new Socket(address->getFamily(),UDP,0));
        return sock;
    }

    Socket::ptr Socket::CreateTCPSocket() {
        Socket::ptr socke(new Socket(IPv4,TCP,0));
        return socke;
    }

    Socket::ptr Socket::CreateUDPSocket() {
        Socket::ptr socke(new Socket(IPv4,UDP,0));
        return socke;
    }

    Socket::ptr Socket::CreateTCPSocket6() {
        Socket::ptr socke(new Socket(IPv6,TCP,0));
        return socke;
    }

    Socket::ptr Socket::CreateUDPSocket6() {
        Socket::ptr socke(new Socket(IPv6,UDP,0));
        return socke;
    }

    Socket::ptr Socket::CreateUnixTCPSocket() {
        Socket::ptr socke(new Socket(UNIX,TCP,0));
        return socke;
    }

    Socket::ptr Socket::CreateUnixUDPSocket() {
        Socket::ptr socke(new Socket(UNIX,UDP,0));
        return socke;
    }


}