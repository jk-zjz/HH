# HH

## 开发环境
centos7 linux  
gcc 9.1.0  
Cmake
## 项目结构
bin  -- 二进制文件目录  
build  -- 中间文件路径  
cmake  --  cmake文件路径  
CMakeLists.txt  --  cmake的定义文件  
hh  --  源文件路径  
tests  --  测试代码
## 日志系统
1）
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
2）
    log日志的使用
```c++
//初始化日志器
hh::Logger::ptr logger(new hh::Logger);
//更改日志格式器
logger->setFormotter(new hh::LogFormotter("%d 123123 %m"));
//添加日志输出地
logger->addAppender(hh::LogAppender::ptr(new hh::FileLogAppender("bbll.txt")));
//初始化事件
hh::LogEvent::ptr logEvent(new hh::LogEvent(logger,logger->getLevel(),__FILE__,__LINE__,0,0,0, time(0)));
//生成日志
logger->Log(hh::LogLevel::DEBUG,logEvent);
```
3）宏的使用
```c++
//第一种 宏使用
hh::Logger::ptr logger(new hh::Logger);
logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));
HH_LOG_INFO(logger, "bsbsbs");
    //bsbsbs  ---  2024-05-02  root 334311 jkjk 
HH_LOG_INFO(logger)<<"bsbsbs"  //这种方式

//第二种 宏使用
hh::Logger::ptr logger(new hh::Logger);
logger->addAppender(hh::LogAppender::ptr(new hh::StdoutLogAppender()));
HH_LOG_FAT_INFO(logger,"sbsbahbdah  %s %d","OK",123);
    //2024-05-02  root 334311 jkjk sbsbahbdah  OK 123
```
3）全局单例模式的使用 & 宏
```c++
    auto i=hh::LoggerMgr::GetInstance()->getLogger(k);
    HH_LOG_INFO(i, "1223");
```
## 配置系统
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
安装 boost库
    
    //下载boost库
    http://www.boost.org/users/download/

    tar -xvf boost_1_63_0.tar.bz2
    cd ./boost_1_63_0
    //编译
    ./bootstrap.sh
    //安装
    sudo ./b2 install



Config 通过 yaml  
安装yaml
    
    //拉取仓库
    git clone https://github.com/jbeder/yaml-cpp.git

    cd yaml-cpp # 进入克隆的文件夹
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
        //默认路径 /usr/local/include/yaml-cpp

对普通类型进行解析 特化

    template<class F,class T>
    class LexicalCast{
    public:
        T operator()(const F& v){
            return boost::lexical_cast<T>(v);
        }
    };

    template<class T,class FromStr=LexicalCast<std::string,T>
    ,class ToStr=LexicalCast<T,std::string>>
    class ConfigVar:public ConfigVarBase{}
对vector 进行特化 (set list 单参数都差不多)
    
    template<class T>
    class LexicalCast<std::string,std::vector<T>> {
    public:
        std::vector<T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::vector<T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss<<i;
                vce.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::vector<T>,std::string> {
    public:
        std::string operator()(const std::vector<T>& var){
            YAML::Node node;
            std::stringstream ss;
            for(auto &i:var){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss<<node;
            return ss.str();
        }
    };
对map进行特化
    
    template<class T>
    class LexicalCast<std::string,std::map<std::string,T>> {
    public:
        std::map<std::string,T> operator()(const std::string &var) {
            YAML::Node node = YAML::Load(var);
            typename std::map<std::string,T> vce;
            for(auto i:node){
                std::stringstream ss;
                ss.str("");
                ss<<i.second;
                vce.insert({i.first.Scalar(),LexicalCast<std::string,T>()(ss.str())});
            }
            return vce;
        }
    };
    template<class T>
    class LexicalCast<std::map<std::string,T>,std::string> {
    public:
        std::string operator()(const std::map<std::string,T> &var) {
            YAML::Node node;
            std::stringstream ss;
            for (auto &i: var) {
                node[i.first]=YAML::Load(LexicalCast<T,std::string>()(i.second));
            }
            ss << node;
            return ss.str();
        }
    };
yaml 文件解析

      logs:
        - name: root
          level: info
          formatter: "%d<%f:%l>[%c:%p]"
          appender:
            - type: FileLogAppender
              file: log.txt
            - type: StdoutLogAppender
        - name: hh
          level: debug
          formatter: "%d<%f:%l>[%c:%p]"
          appender:
            - type: FileLogAppender
              file: test.txt
            - type: StdoutLogAppender
    如果需要读取
    logs[1].appender[0].file
配置文件的主旨   约定>配置  
需在代码中声明，才会读取配置文件

    hh::ConfigVar<std::string>::ptr str = hh::Config::Lookup<std::string>("logs[1].formatter","asd","asda");
    std::cout<<str->getValue();
        //asd
    YAML::Node root = YAML::LoadFile("./log.yml");
    hh::Config::loadFromYaml(root);
    std::cout<<str->getValue();
        //%d<%f:%l>[%c:%p]

    会根据约定替换配置
### 注意配置KEY不区分大小写 
实现自定义类型  
自定义类型需要实现 hh::LexicalCast  
实现后可与基本stl嵌套 因为上面已经实现了一下基本的stl
```c++
class user{
public:
    user(){};
    void setname(const std::string& m_name){
        this->name=m_name;
    }
    void setage( int m_age){
        this->age=m_age;
    }

    const std::string &getName() const {
        return name;
    }

    int getAge() const {
        return age;
    }
    std::string ToString(){
        std::stringstream ss;
        ss<<"name :"<<name<<"age :"<<age;
        return ss.str();
    }

private:
    std::string name;
    int age=0;
};
template<>
class hh::LexicalCast<std::string,user>{
public:
    user operator()(const std::string& var){
        YAML::Node node=YAML::Load(var);
        user u;
        u.setname(node["name"].as<std::string>());
        u.setage(node["age"].as<int>());
        return u;
    }
};
template<>
class hh::LexicalCast<user,std::string>{
public:
    std::string operator()(const user& u){
        YAML::Node node;
        node["name"]=u.getName();
        node["age"]=u.getAge();
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};
//使用
void T(){
    hh::ConfigVar<user>::ptr user_ =hh::Config::Lookup("class.user",user(),"");
    hh::ConfigVar<std::map<std::string,user>>::ptr user_map =hh::Config::Lookup<std::map<std::string,user>>("class.map",{},"");
    hh::ConfigVar<std::vector<user>>::ptr user_vect =hh::Config::Lookup<std::vector<user>>("class.vector",{},"");
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<user_->toString();
    YAML::Node root = YAML::LoadFile("./log.yml");
    hh::Config::loadFromYaml(root);
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<user_->toString();
    for(auto &i:user_map->getValue()){
        std::cout<<i.second.getName();
        std::cout<<i.second.getAge();
        std::cout<<std::endl;
    }
    for(auto &i:user_vect->getValue()){
        std::cout<<i.getName();
        std::cout<<i.getAge();
        std::cout<<std::endl;
    }
}
```
与上面配置的yaml文件格式
```yaml
class:
  user:
      name: root
      age: 18
  map:
      hh01:
        name: root
        age: 18
      hh02:
       name: hh
       age: 20
  vector:
    - name: root
      age: 29
    - name: hy
      age: 90
```
### 配置的事件机制  
当一个配置发生改变的时候,可以返现通知对应代码或回调

    给单独的 ConfigVar 添加配置  是回调函数
    user_->addOcb(10,[](const T& old_valuse,const T& new_valuse){
       std::cout<<old_valuse<<new_valuse<<std::endl;
    });

yml配置日志格式
```yaml 
logs:
  - name: root
    level: info
    formatter: "%d<%f:%l>"
    appender:
      - type: FileLogAppender
        file: ./log.txt
      - type: StdoutLogAppender
  - name: hh
    level: debug
    formatter: "%d<%f:%l>[%m:%p]"
    appender:
      - type: FileLogAppender
        file: log.txt
      - type: StdoutLogAppender
```
使用
```c++
hh::Logger::ptr logger = hh::LoggerMgr::GetInstance()->getLogger(name);
//或者使用宏的方式 HH_LOG_NAME(name);
//以前如果name不存在，就会返回m_root，现在返回的是name，但是使用还是m_root
//会判断name的append是否空，为空使用m_root,当读配置了就会执行name
std::string str("init log");
HH_LOG_INFO(logger,str);
```
## 配置系统整合日志系统使用
```c++
std::string sb("root");
auto roots=HH_LOG_NAME(sb);
HH_LOG_LEVEL_CHAIN(roots,hh::LogLevel::INFO);
//使用默认的日志
YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
hh::Config::loadFromYaml(root);
roots=HH_LOG_NAME(sb);
HH_LOG_LEVEL_CHAIN(roots,hh::LogLevel::INFO);
//使用配置文件中的  name为root的配置
特化了set<log> && 单log
```
通过静态初始化为全局Config类生成对应配置，全局静态配置 
yaml logs文件的样例
```yaml
logs:
  - name: root
    level: info
    formatter: "%d<%f:%l>"
    appender:
      - type: FileLogAppender
        file: ./log.txt
        level: UNKNOWN
        formatter: "%d<%f:%l>root[%m:%p]"
      - type: StdoutLogAppender
        formatter: "%d<%f:%l>[%m:%p]"
  - name: hh
    level: debug
    formatter: "%d<%f:%l>[%m:%p]"
    appender:
      - type: FileLogAppender
        file: log.txt
      - type: StdoutLogAppender
```
```c++
//通过配置文件初始化
YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/log.yml");
hh::Config::loadFromYaml(root);
//通过name root获取格式器使用
auto root=HH_LOG_NAME("root");
HH_LOG_LEVEL_CHAIN(root,hh::LogLevel::INFO);
```
## 线程库
Thread(线程类) ，Mutex(锁......)  
使用的为  
pthread 封装  
pthread pthread_create 

互斥量 mutex
信号量 semaphore

更改log日志配置线程安全  
    
    appender 线程安全  
    logger 线程安全
封装锁  
Mutex 互斥量  
RWMutex 读写互斥量  
SpinLock 自旋锁  
NullMutex 空互斥锁  
NullRWMutex 空读写互斥量  
CASlock 乐观锁
解决 log写文件文件删除问题 周期性(1秒)的打开日志文件

整合log与config文件完毕 提供静态方法序列化内存中的所以配置
```c++
hh:Config::Visit([](hh::ConfigVarBase::ptr var){
    std::cout<<var->toString()<<std::endl;
}
```
序列化删除格式以便于反序列回文件
```yaml
logs:
  - name: hh
    level: UNKNOWN
    appender:
      - type: FileLogAppender
        file: log.txt
      - type: StdoutLogAppender
  - name: root
    level: UNKNOWN
    appender:
      - type: StdoutLogAppender
      - type: FileLogAppender
        file: log.txt
```

## 协程库封装
进入协程模块的准备  
封装断言  中使用函数调用栈序列化以便于后期开发的调试高效

### 协程管理
一个线程对应一个master协程
```
Thread ----> Master_Fiber<----->sub_fiber
                   ^
                   |
                   |
                   v
                sub_fiber
```
### 携程与进程的使用
```c++
/c++
// 定义全局日志记录器
hh::Logger::ptr g_logger = HH_LOG_ROOT();

/**
 * 函数func1: 示范了Fiber的使用，通过Fiber::YieldToHold实现协程的挂起和恢复。
 */
void func1(){
    // 开始运行func1
    HH_LOG_INFO(g_logger,"RUN_IN_ BEGIN");
    // 挂起当前Fiber
    hh::Fiber::YieldToHold();
    // 恢复运行func1
    HH_LOG_INFO(g_logger,"RUN_IN_ END");
    // 再次挂起当前Fiber
    hh::Fiber::YieldToHold();
    // 错误的重复日志条目，应避免在代码中重复或错误的逻辑。
    HH_LOG_INFO(g_logger,"RUN_IN_ END")
}

/**
 * 函数func2: 创建并切换到一个新的Fiber，演示了如何在主Fiber和新创建的Fiber之间进行切换。
 */
void func2(){
    // 获取当前Fiber实例
    hh::Fiber::GetThis();
    // 日志记录：main开始
    HH_LOG_INFO(g_logger, "main begin")
    // 创建一个新的Fiber实例func1
    hh::Fiber::ptr f1(new hh::Fiber(func1));
    // 第一次切换到f1执行
    f1->swapIn();
    // 日志记录：main首次swapIn
    HH_LOG_INFO(g_logger, "main swapIn")
    // 第二次切换回f1执行
    f1->swapIn();
    // 日志记录存在语法错误，应为"main end}"
    HH_LOG_INFO(g_logger,"{main end")
    // 第三次切换回f1执行
    f1->swapIn();
}

/**
 * 主函数：创建并启动多个线程，每个线程执行func2，演示多线程下Fiber的使用。
 * @return int 返回程序执行结果，始终为0。
 */
int main(){
    // 设置当前主线程名称为"main"
    hh::Thread::SetName("main");
    // 存储线程的容器
    std::vector<hh::Thread::ptr>ps;
    // 创建并启动3个线程
    for(int i=0;i<3;i++){
        ps.push_back(std::make_shared<hh::Thread>(&func2,"name_"+std::to_string(i)));
    }
    // 等待所有线程执行完毕
    for(auto& i:ps){
        i->join();
    }
    return 0;
}
```
## 协程调度模块scheduler
跨进程切换协程
```
    1----------N--------M
scheduler -->thread  -->fiber
1.类似于线程池，分配一组线程
2.是一个协程调度器，可以将协程指定到一个线程执行
```
协程调度核心模块
```
run()
while(一直取任务)  是线程池中的每一个线程运行的方法run
加锁取出任务
    判断是否需要指定线程执行 || 是否已经在执行
    如果没有就取出任务
判断任务是一个什么   是一个协程？是一个函数？
1.如果是协程就直接执行
    使用协程切换到前台 判断状态
        是准备就绪状态 重新加入任务队列
        不是结束也不是异常 被打断吧协程状态设置为暂停
    释放空间    
2.如果是函数就创建协程执行
    有空闲直接设置fun
    没有就创建新协程
    协程切换到前台 判断状态
        是准备就绪状态 重新加入任务队列
        是停止状态和异常 释放空间
        否则设置为暂停
3.空闲协程
    不是停止状态 跳出
    不是异常不是停止 设置暂停状态        
```
## IO协程调度模块
```
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
```
```c++
void test_fiber() {
    //所有socket 进行测试
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "110.242.68.66", &addr.sin_addr.s_addr);
    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "add event errno=" << errno << " " << strerror(errno);
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::READ, [](){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "read callback";
        });
        hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::WRITE, [&sock](){
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "write callback";
            //close(sock);
            hh::IOManager::GetThis()->addEvent(sock, hh::IOManager::READ);
        });
    } else {
        HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO) << "else " << errno << " " << strerror(errno);
    }

}
```
## 定时器模块
```
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
hh::Timer::ptr timer;
// 简单使用
void test02(){
    // 创建IOManager对象，并设置线程数
    hh::IOManager iom(2);
    //添加定时器
    timer = iom.addTimer(1000, [](){
        static int count = 0;
        HH_LOG_INFO(g_logger, "hello");
        if(++count == 4) {
            //重新设置定时器，设置为500ms，并且设置为重复定时器
            timer->reset(500, true);
        }
        if(count == 7){
            // 取消定时器
            timer->cancel();
        }
    }, true);
}

```
## HOOK 模块
基本IO函数的hook与socket函数hook

重点是异步变同步
```c++
// hook 模块sleep使用不堵塞
void test01(){
    hh::IOManager iomanager(1);
    iomanager.schedule([](){
        sleep(2);
        HH_LOG_INFO(g_logger,"1");
    });
    iomanager.schedule([](){
        sleep(7);
        HH_LOG_INFO(g_logger,"2");
    });
}


// hook 模块socket使用异步变同步
void test_sock(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        HH_LOG_ERROR(g_logger, "Failed to create socket");
        return;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "122.51.206.19", &addr.sin_addr.s_addr);

    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    if(rt) {
        HH_LOG_ERROR(g_logger, "Failed to connect");
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    if(rt <= 0) {
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR)<< "Failed to send data-="<<sock;
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    if(rt <= 0) {
        HH_LOG_LEVEL_CHAIN(g_logger, hh::LogLevel::ERROR)<< "Failed to receive data";
        return;
    }

    buff.resize(rt);
    HH_LOG_INFO(g_logger, buff);

    // 关闭 socket
    close(sock);
}

```
## socket函数库开发
```
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
封装Address类，封装IPAddress类，封装IPv4Address类，封装IPv6Address类，封装UnixAddress类，

socket  
connect  
accept  bind listen
read write close

// 使用
```c++
// 通过 hh::Address::lookupAnyIPAddress  通过域名获取address地址
hh::Address::ptr addr = hh::Address::lookupAnyIPAddress("www.baidu.com");

// 创建socket  创建的tcpsocket
hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
// 设置端口号
addr->setPort(80);
// 连接
sock->connect(addr)

// 发送请求
std::string buff = "GET / HTTP/1.0\r\n\r\n";
int rt = sock->send(buff.c_str(),buff.size());

// 直接获取，因为send 和recv 被hook了，我们在hook中已经把异步变成同步了
char buff2[4096];
rt = sock->recv(buff2,sizeof(buff2));
```
使用 socket 与 address 与 定时器 配置模块
完成一个服务器，客户端向服务器间隔一秒发送一次请求，服务器回复一个响应

```c++
#include "socket.h"
#include "log.h"
#include "iomanage.h"
#include "config.h"
#include <yaml-cpp/yaml.h>
static hh::Logger::ptr g_logger = HH_LOG_ROOT();
static hh::ConfigVar<std::string>::ptr IP;
static hh::ConfigVar<uint16_t>::ptr PORT;
// 客户端
void test_01(){
    // 初始化ip地址与端口
    hh::IPAddress::ptr addr = hh::IPAddress::Create(IP->getValue().c_str(), PORT->getValue());
    if(!addr){
        return;
    }
    // 创建socket TCP 连接
    hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
    // 连接
    if(!sock->connect(addr)){
        return;
    }
    char buff2[4096];
    // 直接获取，因为send 和recv 被hook了，我们在hook中已经把异步变成同步了
    int rt = sock->recv(buff2,sizeof(buff2));
    if(rt <= 0){
        std::cout<<"recv failed"<<std::endl;
    }else{
        std::cout<<buff2<<std::endl;
    }
    // 关闭连接
    sock->close();
}
void test(){
    HH_LOG_INFO(g_logger,IP->getValue());
    HH_LOG_FAT_INFO(g_logger,"%u",PORT->getValue())
    // 创建ip地址
    hh::IPAddress::ptr addr = hh::Address::lookupAnyIPAddress(IP->getValue());
    addr->setPort(PORT->getValue());
    // 创建socket TCP 连接
    hh::Socket::ptr sock = hh::Socket::CreateTCP(addr);
    // 绑定并且监听
    sock->bind(addr);
    sock->listen();
    while(true){
        // 提取请求
        hh::Socket::ptr sock2 = sock->accept();
        if(sock2){
            sock2->send("hello world\n", strlen("hello world\n"));
            // 获取本地信息
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<sock2->getLocalAddress()->toString();
            // 获取远端信息
            HH_LOG_LEVEL_CHAIN(g_logger,hh::LogLevel::INFO)<<sock2->getRemoteAddress()->toString();
        }else{
            std::cout<<"accept failed"<<std::endl;
            break;
        }
    }
}
int main(){
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/ip.yml");

    // 先约定   因为约定>配置
    IP =hh::Config::Lookup("IP.ip",(std::string)"0.0.0.0","");
    PORT =hh::Config::Lookup("IP.port",(uint16_t)0,"");
    // 在加载配置文件
    hh::Config::loadFromYaml(root);

    HH_LOG_INFO(g_logger,IP->getValue());
    // 创建IOManager
    hh::IOManager iom;
    // 加入任务
    iom.schedule(test);
    // 同步延时秒
    sleep(2);
    // 添加定时器任务，并且循环执行
    iom.addTimer(1000,test_01, true);
    return 0;
}
```


## ByteArray类
```
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

```
```
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
```c++
int main(int argc, char **argv) {
    hh::ByteArray::ptr ba(new hh::ByteArray(4096));
    hh::ByteArray::ptr ba2(new hh::ByteArray(4096));
    std::vector<int32_t> vec;
    for (int32_t i = 0; i < 100; ++i) {
        // 通过未压缩方式存储
        ba->writeFint32(i);
        // 通过压缩方式存储 7bit 与 Zigzag
        ba2->writeInt32(i);
    }
// 设置偏移量到0
    ba->setPosition(0);
    ba2->setPosition(0);

// 读取数据
    for (int32_t i = 0; i < 100; ++i) {
        int32_t v = ba->readFint32();
        HH_LOG_FAT_INFO(g_logger, "v:= %d  i=%d", v, i)
    }
// 文件存储
    ba2->writeToFile("./test.txt");
    hh::ByteArray::ptr ba3(new hh::ByteArray(4096 * 2));
    // 读取文件
    ba3->readFromFile("./test.txt");
    // 因为写入了，所以要设置偏移量为0
    ba3->setPosition(0);
    if (ba3->toString() == ba2->toString()) {
        HH_LOG_INFO(g_logger, "ba3 == ba2");
    }

    return 0;
}
```
## http协议开发

## 分布式协议

## 推荐系统