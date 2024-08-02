//
// Created by 35148 on 2024/7/18.
//

#ifndef HH_SOCKET_STREAM_H
#define HH_SOCKET_STREAM_H
#include "stream.h"
#include "socket.h"
namespace hh{
    class SocketStream : public Stream{
    public:
        typedef std::shared_ptr<SocketStream> ptr;
        SocketStream(hh::Socket::ptr sock, bool owner = true);

        SocketStream();

        ~SocketStream();
        int read(ByteArray::ptr ba, size_t size) override;
        int read(void *buf, size_t size) override;
        int write(ByteArray::ptr ba, size_t size) override;
        int write(const void *buf, size_t size) override;
        void close() override;
        hh::Socket::ptr getSocket() const { return m_socket;}
        bool isConnected();
    protected:
        hh::Socket::ptr m_socket;   // socket
        bool m_owner;               // 是否拥有socket
    };
}
#endif //HH_SOCKET_STREAM_H
