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
add_executable(log_test tests/log_test.cpp ${CPP_G})
target_link_libraries(log_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(log_test)

add_executable(config_test tests/config_test.cc ${CPP_G})
target_link_libraries(config_test
        yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(config_test)

add_executable(pthread_test tests/pthread_test.cc ${CPP_G})
target_link_libraries(pthread_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(pthread_test)

add_executable(utils_test tests/utils_test.cc ${CPP_G})
target_link_libraries(utils_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(utils_test)

add_executable(scheduler_test tests/scheduler_test.cc ${CPP_G})
target_link_libraries(scheduler_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(scheduler_test)

add_executable(iomanager_test tests/iomanager_test.cc ${CPP_G})
target_link_libraries(iomanager_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(iomanager_test)

add_executable(hook_test tests/hook_test.cc ${CPP_G})
target_link_libraries(hook_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(hook_test)

add_executable(address_test tests/address_test.cc ${CPP_G})
target_link_libraries(address_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(address_test)

add_executable(socket_test tests/socket_test.cc ${CPP_G})
target_link_libraries(socket_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(socket_test)


add_executable(bytearray_test tests/bytearray_test.cc ${CPP_G})
target_link_libraries(bytearray_test yaml-cpp::yaml-cpp
        ${PTHREAD}
        dl)
redefine_file_macro(bytearray_test)
