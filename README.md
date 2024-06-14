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

config(配置模块)

thread(线程模块)

fiber(协程模块)

schedule(协程调度模块) 

IOManager(IO协程调度模块)

TimerManager(定时器模块)
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


