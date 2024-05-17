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



## 协程库封装

## socket函数库开发

## http协议开发

## 分布式协议

## 推荐系统