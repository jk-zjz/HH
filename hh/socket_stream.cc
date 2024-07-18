//
// Created by 35148 on 2024/7/18.
//
#include "socket_stream.h"
namespace hh {
    SocketStream::SocketStream(hh::Socket::ptr sock, bool owner)
        :m_socket(sock),m_owner(owner){
    }
    SocketStream::~SocketStream(){
        if(m_owner && m_socket){
            m_socket->close();
        }
    }
    int SocketStream::read(void *buf, size_t size){
        if(!isConnected()){
            return -1;
        }
        return m_socket->recv(buf, size);
    }
    int SocketStream::read(ByteArray::ptr ba, size_t size){
        if(!isConnected()){
            return -1;
        }
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, size);
        int rt = m_socket->recv(&iovs[0], iovs.size());
        if(rt>0){
            ba->setPosition(ba->getPosition() + rt);
        }
        return rt;
    }
    int SocketStream::write(ByteArray::ptr ba, size_t size){
        if(!isConnected()){
            return -1;
        }
        std::vector<iovec> iovs;
        ba->getReadBuffers(iovs, size);
        int rt = m_socket->send(&iovs[0], iovs.size());
        if(rt>0){
            ba->setPosition(ba->getPosition() + rt);
        }
        return rt;
    }
    int SocketStream::write(const void *buf, size_t size){
        if(!isConnected()){
            return -1;
        }
        return m_socket->send(buf, size);
    }
    void SocketStream::close(){
        if(m_socket){
            m_socket->close();
        }
    }
    bool SocketStream::isConnected(){
        return  m_socket && m_socket->isConnected();
    }

}
