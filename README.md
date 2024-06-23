## HH 使用
HH 是一个轻量级的 HTTP 服务器
核心为协程实现，支持多线程，多进程，多协程，支持异步IO，支持HTTP/1.1和HTTP/2
如果需要了解API接口请了解[API接口](https://github.com/jk-zjz/HH/blob/master/API%E6%8E%A5%E5%8F%A3%E6%96%87%E6%A1%A3.md)  
如果需要了解如何使用[使用](https://github.com/jk-zjz/HH/blob/master/hh%E6%9C%8D%E5%8A%A1%E5%99%A8%E6%A1%86%E6%9E%B6.md)  
## HH 编译
centos7 linux  
gcc 9.1.0  
Cmake
## HH 模块
```c++
log(日志模块)

    log4j

    作用域 hh::

    Logger(日志器)
        |
        |------Formotter(日志格式)
        |
        |------LogLevel(日志级别)
        |
        |------LogEvent(日志事件)
        |
        |------LogEventWrap(宏的日志包装类)
        |
    Appender(日志输出地)
```
```c++
config(配置模块)

    作用域 hh::

    ConfigVarBase(配置文件基类)
        |
        |------ConfigVar:ConfigVarBase(配置器)
        |
        |------LexicalCast<std::string,std::list<T>>(配置特化类)
        |
        |------......
        |
    Config(静态配置器)
```
```c++
thread(线程模块)

Thread(线程类) ，Mutex(锁......)  
使用的为  
pthread 封装  
pthread pthread_create 

互斥量 mutex
信号量 semaphore
```
```c++
fiber(协程模块)

Thread ----> Master_Fiber<----->sub_fiber
                   ^
                   |
                   |
                   v
                sub_fiber
```
```c++
schedule(协程调度模块) 
IOManager (epoll) -->scheduler(继承)
     |
     |
     |
     V
  idle(重载)
  
PutMaessage(msg,) 信号量+1 ，single()
meassage_queue
    |
    |---Thread
    |---Thread
        wait()-信号量-1,RecvMessage(msg,)
异步IO,等待数据返回，epoll_wait

流程
poll_wait等待IO事件的发生，当有事件发生时，epoll通知IOManager进行处理。
PutMaessage()将任务放人meassage_queue(队列)
通过single()唤醒wait线程进行RecvMessage()执行

IOManager(IO协程调度模块)
```
```c++
TimerManager(定时器模块)

                    1-N
Timer（定时器类） <-------TimerManager（定时器管理类）
                             ^
                             | 1-1
                             |
（协程调度器）Scheduler<-----IOManager(epoll)（继承与定时器管理，使用定时器接口） 
                |
                | 1->N
                v
             Thread（线程类）
                |
                | N->M
                v
              Fiber（协程类） 
       
TimerManager 通过set红黑树管理定时器，通过写入唤醒epoll_wait(),计算最近定时器时间
并且设置执行，获取定时器过期任务，添加调度器任务列表执行
```
```c++
hook(IO函数与socket函数的hook)

基本IO（read,read......）函数的hook与socket函数hook
```
```c++
socket(socket模块)

    (同主机进程通讯)
    [UnixAddress]
          |                     ----[IPv4Address]
          |        (不同主机通讯) |
      [Address]----[IPAddress]---
          |                     |
          |                     ----[IPv6Address]
          |
       [socket]
        (套接字)   
        
```

## HH 所需的第三方库
[boost库](http://www.boost.org/users/download/)  
```
http://www.boost.org/users/download/

tar -xvf boost_1_63_0.tar.bz2
cd ./boost_1_63_0
//编译
./bootstrap.sh
//安装
sudo ./b2 install
```
[yaml](git clone https://github.com/jbeder/yaml-cpp.git)  
```
//拉取仓库
git clone https://github.com/jbeder/yaml-cpp.git

cd yaml-cpp # 进入克隆的文件夹
mkdir build
cd build
cmake ..
make
sudo make install
    //默认路径 /usr/local/include/yaml-cpp
```


