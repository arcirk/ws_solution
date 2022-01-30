cmake_minimum_required(VERSION 3.19)
project(ws_client VERSION 1.0.1 DESCRIPTION "websocket client")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#set(CMAKE_AUTOMOC ON)

set(TARGET ws_client)

#IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
#    SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/cmake_debug/)
#else()
#    SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/cmake_release/)
#endif()
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_definitions(-DWS_CLIENT_LIBRARY)

list(APPEND SOURCES
        include/beast.h
        include/net.h
        include/ws_session.h
        include/ws_client.h
        src/ws_session.cpp
        src/ws_client.cpp
        src/iws_client.cpp
        include/iws_client.h
        include/stdafx.h
        )
if (WS_CLIENT_STATIC)
    add_library(${TARGET}
            ${SOURCES})
else()
    file(GLOB ARCIRK_SOURCES ../arcirk/src/*.cpp)
    add_library(${TARGET} SHARED
            ${SOURCES}
            ${ARCIRK_SOURCES})
    set_target_properties(ws_client PROPERTIES
            VERSION ${PROJECT_VERSION}
            SOVERSION 1
            PUBLIC_HEADER include/iws_client.h)
endif ()

if (WIN32)
    add_definitions(/MT)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    target_compile_definitions(${TARGET} PRIVATE _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif ()

            ##SHARED
            #add_library(ws_client SHARED
            #        include/beast.h
            #        include/net.h
            #        include/ws_session.h
            #        include/ws_client.h
            #        src/ws_session.cpp
            #        src/ws_client.cpp
            #        src/iws_client.cpp
            #        include/iws_client.h
            #        ../arcirk/src/arcirk.cpp
            #        ../arcirk/src/arcirk_static.cpp
            #        ../arcirk/src/arcirk_json.cpp
            #        ../arcirk/src/arcirk_ip.cpp
            #        ../arcirk/src/arcirk_message.cpp
            #        ../arcirk/src/arcirk_settings.cpp
            #        include/stdafx.h)
            #
            #set_target_properties(ws_client PROPERTIES
            #        VERSION ${PROJECT_VERSION}
            #        SOVERSION 1
            #        PUBLIC_HEADER include/iws_client.h)
if (WIN32)
    link_directories(
            ${ARCIRK_LIB}
            ${WDC_LIB})
endif ()

if (WIN32)
    find_package(Boost REQUIRED COMPONENTS regex)
    target_link_libraries(${TARGET} PRIVATE
            bcrypt.lib libwdc arcirk)

    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
    )

else()
    SET(CMAKE_CXX_FLAGS -pthread)
    find_package(Boost COMPONENTS regex filesystem system thread REQUIRED)
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
    )

    target_link_libraries(${TARGET} PRIVATE
            Boost::filesystem
            Boost::system
            Boost::thread
            -static-libstdc++)
endif()

#if (UNIX)
#    SET(CMAKE_CXX_FLAGS -pthread)
#endif ()
#
#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
#else()
#    set (NMOS_CPP_SYSTEM_LIBS )
#endif()
#if (UNIX)
#find_package(Boost REQUIRED COMPONENTS thread regex filesystem)
#else()
#find_package(Boost REQUIRED COMPONENTS regex)
#endif()
#
#include_directories(
#        "../arcirk/include"
#        ${Boost_INCLUDE_DIRS})

#if (UNIX)
#target_link_libraries(ws_client
#        ${Boost_LIBRARIES}
#        Boost::thread
#        Boost::filesystem
#        ${NMOS_CPP_SYSTEM_LIBS}
#        )
#else()
#    target_link_libraries(ws_client  PRIVATE
#        ${Boost_LIBRARIES}
#        ${NMOS_CPP_SYSTEM_LIBS}
#        )
#endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${TARGET} PRIVATE /W4 /w44265 /w44061 /w44062 )
endif()

if(WIN32)
#[[    ADD_CUSTOM_COMMAND(
            TARGET ${TARGET}
            POST_BUILD
            COMMENT "Update lib..."
            COMMAND ${CMAKE_COMMAND} -E rm ../../ws_admin_qt/ws_admin_qt/cmake-build-debug/${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX})


    ADD_CUSTOM_COMMAND(
            TARGET ${TARGET}
            POST_BUILD
            COMMENT "Copying lib..."
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX}
            ../../ws_admin_qt/ws_admin_qt/cmake-build-debug
    )]]
endif ()
