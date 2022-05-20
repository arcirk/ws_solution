cmake_minimum_required (VERSION 3.21)

project (ws_service)
set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_service)

if (NOT WIN32)
    message(FATAL_ERROR "This application for Windows os only!")
endif(NOT WIN32)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)
#        set (Boost_USE_MULTITHREADED ON)
#
foreach (flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

    if (${flag_var} MATCHES "/MD")
        string (REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif (${flag_var} MATCHES "/MD")

endforeach(flag_var)

set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRTD")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_definitions(/MT)

SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(ARCIRK_LIB ../arcirk/cmake-build-debug)
set(ARCIRK_INCLUDE ../arcirk/include)
set(WS_SERVER_INCLUDE ../ws_server/include)

link_directories(
        ${ARCIRK_LIB}
        )

file(GLOB SERVICE_CPP "*.cpp")
file(GLOB SERVICE_H "*.h")

list(APPEND SOURCES
        ../ws_server/include/websocket_session.hpp
        ../ws_server/include/shared_state.hpp
        ../ws_server/include/net.hpp
        ../ws_server/include/listener.hpp
        ../ws_server/include/http_session.hpp
        ../ws_server/include/beast.hpp
        ../ws_server/http_session.cpp
        ../ws_server/listener.cpp
        ../ws_server/shared_state.cpp
        ../ws_server/websocket_session.cpp
        ../ws_server/include/subscriber.h
        ../ws_server/subscriber.cpp
        ../ws_server/include/public_channels.h
        ../ws_server/sqlite/sqlite3.c
        ../ws_server/sqlite/sqlite3.h
        ../ws_server/include/base.h
        ../ws_server/src/base.cpp)

add_executable(${TARGET}
        ${SERVICE_CPP}
        ${SERVICE_H}
        ${SOURCES} ws_service.cpp)

target_include_directories(${TARGET} PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${ARCIRK_INCLUDE}
        ${WS_SERVER_INCLUDE}
        )
find_package(Boost REQUIRED COMPONENTS regex)

target_link_libraries(${TARGET} PRIVATE
        bcrypt.lib arcirk)

include_directories (
        ${Boost_INCLUDE_DIRS}
        ${ARCIRK_INCLUDE}
)

add_custom_command(TARGET ws_service PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ws_server/app/ ${CMAKE_BINARY_DIR})