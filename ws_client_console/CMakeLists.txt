cmake_minimum_required(VERSION 3.19)
project(ws_client_console)

set(CMAKE_CXX_STANDARD 20)

set(WS_CLIENT_INCLUDE ../ws_client/include)
set(WS_CLIENT_LIB ../ws_client/cmake-build-debug)
set(CHARED_INCLUDE ../chared)
set(ARCIRK_INCLUDE ../arcirk/include)
set(ARCIRK_LIB ../arcirk/cmake-build-debug)

add_executable(ws_client_console src/main.cpp
        ../chared/server_response.cpp
        ../chared/server_response.h
        )

if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif ()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

find_package(Boost REQUIRED COMPONENTS thread filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(ws_client_console PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ws_client_console PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ws_client_console PRIVATE /W4 /w44265 /w44061 /w44062)
endif()


link_directories(
        ${ARCIRK_LIB}
        ${WS_CLIENT_LIB})

include_directories("../ws_client/include")
include_directories("../chared")
#link_directories(../ws_client/cmake-build-debug)

target_link_libraries(ws_client_console
        ${Boost_LIBRARIES}
        arcirk
        ws_client
        ${NMOS_CPP_SYSTEM_LIBS})