cmake_minimum_required(VERSION 3.29)
include(cmake/utils.cmake)
project(HH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(LINK_K "/usr/lib64/ /usr/local/lib/ /usr/lib/")

# 读取环境变量中的CXXFLAGS，并设置其他的编译器选项
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# 设置生成的可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 查找yaml库
find_package(yaml-cpp REQUIRED)
find_library(PTHREAD pthread ${LINK_K})
# 读取./hh 下的源文件
aux_source_directory("./hh" CPP_G)

# 添加头文件目录
include_directories("./hh/include/"
                    ${YAML_CPP_INCLUDE_DIR})


# 添加可执行文件HH，并指定源文件
add_executable(test tests/main.cpp ${CPP_G})
target_link_libraries(test yaml-cpp::yaml-cpp
                    ${PTHREAD})
redefine_file_macro(test)

add_executable(config tests/main2.cc ${CPP_G})
target_link_libraries(config
                    yaml-cpp::yaml-cpp
                    ${PTHREAD})
redefine_file_macro(config)


add_executable(pthread tests/pthread.cc ${CPP_G})
target_link_libraries(pthread yaml-cpp::yaml-cpp
                    ${PTHREAD})
redefine_file_macro(pthread)