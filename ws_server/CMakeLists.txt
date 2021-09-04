cmake_minimum_required(VERSION 3.19)
project(ws_server)

set(CMAKE_CXX_STANDARD 17)
if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif()

#SET(CMAKE_CXX_FLAGS -lsqlite3)
#add_compile_options(-l sqlite3)
#add_compile_options(-l ldl)

#if(CMAKE_DL_LIBS)
#    target_link_libraries(ws_server ${CMAKE_DL_LIBS})
#endif()

add_executable(ws_server
        main.cpp
        include/websocket_session.hpp
        include/shared_state.hpp
        include/net.hpp
        include/listener.hpp
        include/http_session.hpp
        include/beast.hpp
        http_session.cpp
        listener.cpp
        shared_state.cpp
        websocket_session.cpp
        include/subscriber.h
        subscriber.cpp
        include/public_channels.h
        sqlite/sqlite3.c
        sqlite/sqlite3.h
        include/base.h
        src/base.cpp)

find_package(Boost REQUIRED COMPONENTS regex)
#find_package (SQLite3 REQUIRED CONFIG)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("../arc_json")
#include_directories(${SQLITE3_INCLUDE_DIRS})
target_link_libraries(ws_server PRIVATE
        ${Boost_LIBRARIES}
        arc_json
        ${CMAKE_DL_LIBS})
        #${SQLITE3_LIBRARIES}

add_custom_command(TARGET ws_server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ws_server/app $<TARGET_FILE_DIR:ws_server>)

