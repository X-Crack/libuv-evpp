cmake_minimum_required(VERSION 3.17)
project(evpp)

set(CMAKE_CXX_STANDARD 20)
set(CXX_FLAGS
        -g
        -fPIC
        -Wall
        -Wno-unused-parameter
        -Wno-unused-function
        -Wunused-variable
        -Wunused-value
        -Wshadow
        -Wcast-align
        -Wwrite-strings
        -Wsign-compare
        -Winvalid-pch
        -fms-extensions
        -Wfloat-equal
        -Wextra
        -std=c++20)

link_directories(. /usr/local/lib)
include_directories(/usr/local/include)
include_directories(/usr/local/include/glog)
include_directories(. source/deps/concurrentqueue)
include_directories(. source/include)
include_directories(. source source/event)
aux_source_directory(source src)
aux_source_directory(source/event src_event)
add_executable(evpp ${src} ${src_event})

target_link_libraries(evpp pthread uv glog)