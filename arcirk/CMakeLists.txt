cmake_minimum_required(VERSION 3.20)
project(arcirk)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS                " ${CMAKE_CXX_FLAGS_INIT} -std=c++11 -fPIC")
#set(CMAKE_CXX_FLAGS_DEBUG          "-g -DDEBUG")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DDEBUG")

add_library(arcirk src/arcirk.cpp src/arcirk_static.cpp src/arcirk_json.cpp src/arcirk_ip.cpp src/arcirk_message.cpp ../build-ws_systray-Desktop_Qt_6_2_1_GCC_64bit-Debug/ui_window.h)

find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(arcirk ${Boost_LIBRARIES})