cmake_minimum_required(VERSION 3.29)
include(cmake/utils.cmake)
project(HH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置生成的可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
# 查找yaml库
find_package(yaml-cpp REQUIRED)
find_package(Boost COMPONENTS url REQUIRED)

# 读取源文件
aux_source_directory("./hh" CPP_G)
aux_source_directory("./hh/http" HTTP_SOURCES)

# 添加头文件目录
include_directories(
        "./hh/include/"
        "./hh/http/include"
        ${YAML_CPP_INCLUDE_DIRS}
)

# 定义一个函数来处理可执行文件的创建和链接
function(add_test_executable name srcs)
    add_executable(${name} ${srcs} ${CPP_G} ${HTTP_SOURCES})
    target_link_libraries(${name} PRIVATE yaml-cpp::yaml-cpp pthread dl Boost::url)
    redefine_file_macro(${name}) # 如果这是必要的，确保宏被正确定义
endfunction()

# 使用上述函数添加所有可执行文件
add_test_executable(log_test tests/log_test.cpp)
add_test_executable(config_test tests/config_test.cc)
add_test_executable(pthread_test tests/pthread_test.cc)
add_test_executable(utils_test tests/utils_test.cc)
add_test_executable(scheduler_test tests/scheduler_test.cc)
add_test_executable(iomanager_test tests/iomanager_test.cc)
add_test_executable(hook_test tests/hook_test.cc)
add_test_executable(address_test tests/address_test.cc)
add_test_executable(socket_test tests/socket_test.cc)
add_test_executable(bytearray_test tests/bytearray_test.cc)
add_test_executable(http_test tests/http_test.cc)
add_test_executable(HttpParser_test tests/HttpParser_test.cc)
add_test_executable(Tcp_server_test tests/Tcp_server_test.cc)
add_test_executable(echo_server examples/echo_server.cc)
add_test_executable(httpserver_test tests/httpserver_test.cc)
add_test_executable(httpConnection_test tests/httpConnection_test.cc)
add_test_executable(httpUrl_test tests/httpUrl_test.cc)

# 如果需要安装可执行文件，可以添加如下代码
# install(TARGETS log_test config_test ... DESTINATION bin)
