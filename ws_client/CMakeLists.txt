cmake_minimum_required(VERSION 3.19)
project(ws_client VERSION 1.0.1 DESCRIPTION "websocket client")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(TARGET ws_client)

#SHARED
add_library(ws_client SHARED
        include/beast.h
        include/net.h
        include/ws_session.h
        include/ws_client.h
        src/ws_session.cpp
        src/ws_client.cpp
        src/iws_client.cpp
        include/iws_client.h
        ../arcirk/src/arcirk.cpp
        ../arcirk/src/arcirk_static.cpp
        ../arcirk/src/arcirk_json.cpp
        ../arcirk/src/arcirk_ip.cpp
        ../arcirk/src/arcirk_message.cpp
        ../arcirk/src/arcirk_settings.cpp)

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
if (UNIX)
find_package(Boost REQUIRED COMPONENTS thread regex filesystem)
else()
find_package(Boost REQUIRED COMPONENTS regex)
endif()

include_directories(
        "../arcirk/include"
        ${Boost_INCLUDE_DIRS})

if (UNIX)
target_link_libraries(ws_client
        ${Boost_LIBRARIES}
        Boost::thread
        Boost::filesystem
        ${NMOS_CPP_SYSTEM_LIBS}
        )
else()
    target_link_libraries(ws_client  PRIVATE
        ${Boost_LIBRARIES}
        ${NMOS_CPP_SYSTEM_LIBS}
        )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(ws_client PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ws_client PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ws_client PRIVATE /W4 /w44265 /w44061 /w44062 )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
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
