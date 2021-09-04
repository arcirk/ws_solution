cmake_minimum_required(VERSION 3.19)
project(ws_drv_test)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(ws_drv_test main.cpp)

include_directories("../../ws_drv/src")
target_link_libraries(ws_drv_test PUBLIC
        ws_drv)