cmake_minimum_required(VERSION 3.19)
project(ws_client VERSION 1.0.1 DESCRIPTION "websocket client")

set(CMAKE_CXX_STANDARD 17)

#SHARED
add_library(ws_client SHARED
        include/beast.h
        include/net.h
        include/ws_session.h
        include/ws_client.h
        include/global.h
        src/ws_session.cpp
        src/ws_client.cpp
        src/iws_client.cpp
        include/iws_client.h)

set_target_properties(ws_client PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER include/iws_client.h)

if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif ()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

find_package(Boost REQUIRED COMPONENTS thread)
find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries( ws_client
        ${Boost_LIBRARIES}
        Boost::thread
        ${NMOS_CPP_SYSTEM_LIBS}
        )

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(ws_client PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ws_client PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ws_client PRIVATE /W4 /w44265 /w44061 /w44062 )
endif()
