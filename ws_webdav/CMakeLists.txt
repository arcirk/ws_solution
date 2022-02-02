cmake_minimum_required(VERSION 3.20)
project(ws_webdav_test VERSION 1.0.1 DESCRIPTION "webdav client")

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_webdav_test)

add_executable(${TARGET} main.cpp)

find_package(Boost REQUIRED COMPONENTS regex filesystem)
include_directories(../ws_webdav/lib/include)
link_directories(../ws_webdav/lib/cmake-build-debug)

include_directories(
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${TARGET}
        ${Boost_LIBRARIES}
        ws_webdav
        )