cmake_minimum_required(VERSION 3.19)
project(ws_client_console)

set(CMAKE_CXX_STANDARD 20)

add_executable(ws_client_console src/main.cpp
        ../chared/server_response.cpp
        ../chared/server_response.h
        ../ws_client/include/net.h
        ../ws_client/include/beast.h
        ../ws_client/include/ws_session.h
        ../ws_client/include/ws_client.h
        ../ws_client/src/iws_client.cpp
        ../ws_client/src/ws_session.cpp
        ../ws_client/src/ws_client.cpp
        ../arcirk/include/arcirk.h
        ../arcirk/src/arcirk.cpp
        ../arcirk/src/arcirk_ip.cpp
        ../arcirk/src/arcirk_json.cpp
        ../arcirk/src/arcirk_message.cpp
        ../arcirk/src/arcirk_settings.cpp
        ../arcirk/src/arcirk_static.cpp
        )

if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif ()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

find_package(Boost REQUIRED COMPONENTS thread)
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


include_directories("../ws_client/include")
include_directories("../chared")
#link_directories(../ws_client/cmake-build-debug)

target_link_libraries(ws_client_console
        ${Boost_LIBRARIES}
        ${NMOS_CPP_SYSTEM_LIBS})