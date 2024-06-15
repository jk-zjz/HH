//
// Created by 35148 on 2024/6/14.
//
#ifndef HH_HOOK_H
#define HH_HOOK_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "fd_manager.h"
namespace hh {
    bool is_hook_enable();

    void set_hook_enable(bool flag);
}
// 函数重定义 告诉编译器使用C语言的风格
extern "C" {
/**
 * sleep 相关函数hook
 */
//设置函数签名  sleep_f代表原来的库函数
typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

//设置函数签名  usleep_f代表原来的库函数
typedef int (*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;

typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
extern nanosleep_fun nanosleep_f;
/**
 * socket 相关函数hook
 */
// 创建socket
typedef int (*socket_fun)(int domain, int type, int protocol);
extern socket_fun socket_f;
// 连接
typedef int (*connect_fun)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_fun connect_f;
// 监听
typedef int (*accept_fun)(int s, struct sockaddr *addr, socklen_t *addrlen);
extern accept_fun accept_f;
/**
 * 读相关函数hook
 */
 // 读
typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
extern read_fun read_f;

// 可以分缓冲区写入到不同的地方
typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
extern readv_fun readv_f;

// 读 flags可以用来设置一些标志位最常用的为MSG_PEEK，读取数据但不删除，MSG_OOB，读取带外数据，MSG_DONTWAIT，非阻塞等
typedef ssize_t (*recv_fun)(int sockfd, void *buf, size_t len, int flags);
extern recv_fun recv_f;

// 读 可以存储发送方的地址src_addr
typedef ssize_t (*recvfrom_fun)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr,
                                socklen_t *addrlen);
extern recvfrom_fun recvfrom_f;

// 读 msghdr 结构体的指针，该结构体描述了消息的缓冲区和辅助数据。
typedef ssize_t (*recvmsg_fun)(int sockfd, struct msghdr *msg, int flags);
extern recvmsg_fun recvmsg_f;
/**
* 写相关函数hook
*/
// 写
typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
extern write_fun write_f;
// 可以分缓冲区写入到不同的地方
typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
extern writev_fun writev_f;
// 写 flags可以用来设置一些标志位最常用的为MSG_PEEK，读取数据但不删除，MSG_OOB，读取带外数据，MSG_DONTWAIT，非阻塞等
typedef ssize_t (*send_fun)(int sockfd, const void *buf, size_t len, int flags);
extern send_fun send_f;
// 写 可以存储发送方的地址src_addr
typedef ssize_t (*sendto_fun)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
                              socklen_t addrlen);
extern sendto_fun sendto_f;
// 写 msghdr 结构体的指针，该结构体描述了消息的缓冲区和辅助数据。
typedef ssize_t (*sendmsg_fun)(int sockfd, const struct msghdr *msg, int flags);
extern sendmsg_fun sendmsg_f;

// 关闭文件
typedef int (*close_fun)(int fd);
extern close_fun close_f;
/**
 * socket操作相关函数hook
 */
 // 设置文件句柄访问状态
 typedef int (*fcntl_fun)(int fd, int cmd, ... /* arg */ );
 extern fcntl_fun fcntl_f;

 // 获取文件句柄访问状态
 typedef int (*ioctl_fun)(int fd, unsigned long request, ...);
 extern ioctl_fun ioctl_f;

// 获取文件句柄访问状态
typedef int (*getsockopt_fun)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern getsockopt_fun getsockopt_f;

// 设置文件句柄访问状态
typedef int (*setsockopt_fun)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
extern setsockopt_fun setsockopt_f;
}


#endif //HH_HOOK_H
