//
// Created by 35148 on 2024/6/27.
//
#include "socket.h"
#include "fd_manager.h"
#include "log.h"
#include "macro.h"
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

        return false;
    }

    bool Socket::connect(const Address::ptr &address, int timeout_ms) {
        return false;
    }

    bool Socket::listen(int backlog) {
        return false;
    }

    bool Socket::close() {
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
        return 0;
    }

    int Socket::send(const iovec *buffers, size_t length, int flags) {
        return 0;
    }

    int Socket::sendTo(const void *buffer, size_t length, const Address::ptr to, int flags) {
        return 0;
    }

    int Socket::sendTo(const iovec *buffer, size_t length, const Address::ptr to, int flags) {
        return 0;
    }

    int Socket::recv(void *buffer, size_t length, int flags) {
        return 0;
    }

    int Socket::recv(iovec *buffer, size_t length, int flags) {
        return 0;
    }

    int Socket::recvFrom(void *buffer, size_t length, Address::ptr &from, int flags) {
        return 0;
    }

    int Socket::recvFrom(iovec *buffer, size_t length, Address::ptr &from, int flags) {
        return 0;
    }

    Address::ptr Socket::getLocalAddress() {
        return hh::Address::ptr();
    }

    Address::ptr Socket::getRemoteAddress() {
        return hh::Address::ptr();
    }

    bool Socket::isValid() const {
        return false;
    }

    int Socket::getError() {
        return 0;
    }

    std::ostream &Socket::dump(std::ostream &os) const {

    }

    bool Socket::cancelRead() {
        return false;
    }

    bool Socket::cancelWrite() {
        return false;
    }

    bool Socket::cancelAccept() {
        return false;
    }

    bool Socket::cancelAll() {
        return false;
    }

    void Socket::initSock() {
        int val =1;
        // 设置端口复用
        setOption(SOL_SOCKET, SO_REUSEADDR, val);
        // 判断是否为流式套接字，还有一种是UDP的
        if(m_type == SOCK_STREAM){
            setOption(SOL_SOCKET, SO_KEEPALIVE, val);
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
}