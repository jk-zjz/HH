cmake_minimum_required(VERSION 3.29)

project(HH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 读取环境变量中的CXXFLAGS，并设置其他的编译器选项
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -Wall -Wno-deprecated -Werror -Wno-unused-function")

# 设置生成的可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 读取头文件的目录（此处似乎遗漏了实际读取头文件的代码）
# 例如：include_directories("./hh/include/")

# 读取./hh 下的源文件
aux_source_directory("./hh" CPP_G)

# 添加头文件目录
include_directories("./hh/include/")

# 添加可执行文件HH，并指定源文件
add_executable(HH tests/main.cpp ${CPP_G})