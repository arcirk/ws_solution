cmake_minimum_required(VERSION 3.19)
project(test_channels_node)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(test_channels_node main.cpp)

find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(test_channels_node ${Boost_LIBRARIES})

# Подключение библиотеки, указываем, откуда брать заголовочные файлы
include_directories("../../arc_json")
# А также указываем зависимость от статической библиотеки
target_link_libraries(test_channels_node arc_json)

