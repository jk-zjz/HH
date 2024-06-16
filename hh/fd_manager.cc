//
// Created by 35148 on 2024/6/15.
//
#include <sys/stat.h>
#include "fd_manager.h"
namespace hh {

    FdCtx::FdCtx(int fd):
    m_isInited(false),
    m_isClosed(false),
    m_sysNonblock(false),
    m_userNonblock(false),
    m_isSocket(false),
    m_sockfd(fd){
        m_timeout[0]=m_timeout[1]=-1;
        init();

    }

    FdCtx::~FdCtx() {

    }

    bool FdCtx::close() {
        return false;
    }

    bool FdCtx::init() {
        // 判断是否初始化
        if(m_isInited){
            return true;
        }
        m_timeout[0]=m_timeout[1]=-1;
        struct stat fd_stat;
        // 获取句柄文件状态
        if(fstat(m_sockfd,&fd_stat)==-1){
            // 失败
            m_isInited=false;
            m_isSocket=false;
            return false;
        }else{
            m_isInited=true;
            // 判断是否为套接字 S_ISSOCK宏
            m_isSocket= S_ISSOCK(fd_stat.st_mode);
        }
        if(m_isSocket){
            // 是套接字 设置非阻塞
            int flags=fcntl_f(m_sockfd,F_GETFL,0);
            if(!(flags&O_NONBLOCK)){
                flags|=O_NONBLOCK;
                fcntl_f(m_sockfd,F_SETFL,flags);
            }
            m_sysNonblock=true;
        }else{
            m_sysNonblock=false;
        }
        m_userNonblock=false;
        m_isClosed=false;
        return m_isInited;
    }

    void FdCtx::setTimeout(int type, uint64_t v) {
        if(type==SO_RCVTIMEO){
            m_timeout[0]=v;
        }else{
            m_timeout[1]=v;
        }
    }

    FdManager::FdManager() {
        m_datas.resize(64);
    }

    FdCtx::ptr FdManager::get(int fd, bool auto_create) {
        RWMutexType::ReadLock lock(m_mutex);
        if(fd>=(int)m_datas.size()){
            // 如果不自动创建就返回
            if(!auto_create){
                return nullptr;
            }
        }else{
            if(!auto_create || m_datas[fd]){
                return m_datas[fd];
            }
        }
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        FdCtx::ptr ctx(new FdCtx(fd));
        if(fd>=(int)m_datas.size()){
            m_datas.resize(fd*1.5);
        }
        m_datas[fd]=ctx;
        return ctx;
    }

    void FdManager::del(int fd) {
        RWMutexType::WriteLock lock(m_mutex);
        if(fd>=(int)m_datas.size()){
            return;
        }
        m_datas[fd].reset();
    }
}