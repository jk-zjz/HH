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

作用域 hh::
    
Thread(线程类) ,Mutex(锁......)  
使用的为  
pthread 封装  
pthread pthread_create 

互斥量 mutex
信号量 semaphore
```

```c++
fiber(协程模块)

    作用域 hh::
    
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
通过IOManager 继承scheduler,通过匿名管道实现协程调度
    当有任务到达,会唤醒向匿名管道内写入

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
        
        
封装Address类，封装IPAddress类，封装IPv4Address类，封装IPv6Address类，封装UnixAddress类，

socket  
connect  
accept  bind listen
read write close        
```


```c++

ByteArray 字节压缩模块

压缩基础数据类型32int uint  64int uint
使用 7bit 压缩数据 与 Zigzag 压缩数据
7bit 是将数据循环压缩到小于7bit结束
    每一次将数据 （数据 & 01111111） | 10000000 得到每一位，存储，然后继续循环
    直到数据<0x80结束
    最后吧小于0x80的数据存储
    
Zigzag 是将无符号的数据或者有符号的数据都转换为无符号
    (-数据)<<1位-1
    如果数据都最后一位是1，那么说明数据一开始就是负数，那么-数据>>1位+1
    如果数据最后一位是0，那么说明数据一开始就是正数，那么数据>>1位
    -((-(-2))<<1-1) = 3   101  >> 1  10 +1 = -11 就还原 -2
            
            
ByteArray    | m_root 主节点 用来标准第一个节点
    |        | m_cur 当前使用节点
    |--------| m_size ByteArray实体中存储的数据大小
    |        | m_endian  大小端
    |        | m_capacity 这一个实体类的大小
    |        | m_position 当前处理位置
    |        | m_baseSize 一个节点的大小
    |       
    |--------|可以存储未压缩的 8bit 16bit 32bit 64bit
             |可以存储压缩的  32bit 64bit
             |可以将节点数据存储到文件
             |以及读取文件数据
             |使用tostring
```
## http协议开发
```c++
HTTP/1.1 -API  格式

HttpRequest     收请求
HttpResponse    发请求

url 格式
http://www.hhap.cn:80/page/xxx?id=10#fr
  |         |       |      |     |   |
http 协议    |       |      |     |   |
        www.hhap.cn主机名   |     |   |
                    |      |     |   |
                80 端口    |      |   |
                    /page/xxx 路径|   |
                            id=10 参数|
                                   #fr 锚点
                                   
                    ragel 有限状态机 做http的解析
                            通过ragel解析http请求
                                    |
HttpResponse->解析HttpResponseParser-|-解析到HttpRequestParser<-HttpRequest
                                    |
                              使用回调函数处理请求
```
```c++
封装Tcp_Server 基类
        |
        |----------提供绑定(虚函数)
        |----------提供启动跳转(虚函数)
        |----------提供handleClient(重点虚函数)
        |----------提供startAccept(重点虚函数)

提供 echo_server 测试  ------>   tcp_server
        |
        |-------> 重写handleClient,完成回复代码
```
```c++
封装stream流便于 file && socket
      |
      |------提供多种写入读取方式
      |------提供固定读入写出

        
      SocketStream <----- stream
            ^
            |
            |
            |
        HttpSession
            |------->是server accept 的socket
            
提供 http_server 测试 ----->   http_server  
        |
        |-------> 重写handleClient,完成回复代码
```
```c++
## Stream 针对文件/socket封装
read/write // 读指定大小不强制  
readFixSize/WriteFixSize // 读取指定大小强制

HttpSession/HttpConnection
server.accept socket 是 HttpsSession
client.connect socket 是 HttpConnection

HttpServer : TcpServer

                             servlet 的回调函数
             Servlet  <----- FunctionServlet
                |
                |
        ServletDispatcher
          Servlet管理类
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

[yaml](https://github.com/jbeder/yaml-cpp.git)
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
[ragel](https://www.colm.net/open-source/ragel/)
```
可以自定义下载与安装
ragel 下载依赖需要先安装 colm

也可以使用 yum安装

yum intsall ragel -y;

```

