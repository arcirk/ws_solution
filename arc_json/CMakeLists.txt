cmake_minimum_required(VERSION 3.19)
project(arc_json)

set(CMAKE_CXX_STANDARD 17)

add_library(arc_json arc_json.cpp arc_json.h global.h ip.h)

find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(arc_json ${Boost_LIBRARIES})