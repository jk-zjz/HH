#!/bin/bash
# 安装 wget 和 git 如果它们不存在
install_if_missing() {
    local cmd="$1"
    if ! command -v "$cmd" &> /dev/null; then
        yum -y install "$cmd"
        if [ $? -ne 0 ]; then
            echo "安装 $cmd 失败"
            exit 1
        fi
    fi
}

install_if_missing wget
install_if_missing git

# 安装 Boost
install_boost() {
    if [ -d "/usr/local/include/boost" ]; then
        echo "Boost 已安装"
        return 0
    fi
    wget https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz
    if [ $? -ne 0 ]; then
        echo "下载 Boost 失败"
        exit 1
    fi
    tar -xzf boost_1_85_0.tar.gz
    cd boost_1_85_0 || exit 1
    ./bootstrap.sh || { echo "执行 bootstrap.sh 失败"; exit 1; }
    ./b2 || { echo "编译 Boost 失败"; exit 1; }
    ./b2 install || { echo "安装 Boost 失败"; exit 1; }
    cd ..
    rm -rf boost_1_85_0
    rm -rf boost_1_85_0.tar.gz
}

# 安装 yaml-cpp
install_yamlcpp() {
    if [ -d "/usr/local/include/yaml-cpp" ]; then
        echo "yaml-cpp 已安装"
        return 0
    fi
    git clone https://github.com/jbeder/yaml-cpp.git yaml
    if [ ! -d "yaml" ]; then
        echo "克隆 yaml-cpp 失败"
        exit 1
    fi
    cd yaml || exit 1
    mkdir build && cd build || exit 1
    cmake .. && make && make install || { echo "构建和安装 yaml-cpp 失败"; exit 1; }
    cd .. && cd .. && rm -rf yaml
}

install_boost
install_yamlcpp
echo "OK   cmake && make 可以继续"