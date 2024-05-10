#!/bin/bash
if ! command -v wget &> /dev/null; then
    yum -y install wget
      if [ $? -ne 0 ]; then
          echo "wget 下载失败"
          exec 1
      fi
fi
if ! command -v git &> /dev/null; then
    yum -y install git
      if [ $? -ne 0 ]; then
          echo "git 下载失败"
          exec 1
      fi
fi

function install_boost() {
  if [ -d "/usr/local/include/boost" ]; then
       return 0;
  fi
  wget https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz
  if [ $? -ne 0 ]; then
      echo "下载失败"
      exec 1
  fi
  tar -xzf boost_1_85_0.tar.gz
  cd boost_1_85_0
  ./bootstrap.sh
  if [ $? -ne 0 ]; then
      echo "bootstrap.sh 执行失败"
      return 1
  fi
  ./b2
  if [ $? -ne 0 ]; then
      echo "编译Boost失败"
      return 1
  fi
  ./b2 install
  if [ $? -ne 0 ]; then
      echo "安装Boost失败"
      return 1
  fi
  cd ..
  rm -rf boost_1_85_0
  rm -rf boost_1_85_0.tar.gz
}
function install_yamlcpp() {
    if [ -d "/usr/local/include/yaml-cpp" ]; then
        return 0;
    fi
    git clone https://github.com/jbeder/yaml-cpp.git yaml
    if [ -d "yaml" ]; then
      cd yaml
      mkdir build
      cd build
      camke && make && make install
      if [ $? -ne 0 ]; then
        echo "yaml-cpp 失败"
      fi
      cd .. && cd .. && rm -rf yaml
    else
      echo "克隆失败"
    fi
}
install_boost
install_yamlcpp
echo "OK   cmake && make "