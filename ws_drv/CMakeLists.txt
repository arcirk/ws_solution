cmake_minimum_required(VERSION 3.21)
project(ws_drv)

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_drv)

if(WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

option(CASE_INSENSITIVE "Case insensitive method names" OFF)
option(STATIC_CRT "Static CRT linkage" OFF)
option(OUT_PARAMS "Support output parameters" OFF)

#target_precompile_headers(${TARGET} PRIVATE F:/qt_sr/infactum/ws_client/include/stdafx.h)

list(APPEND SOURCES
        src/addin.def
        src/stdafx.h
        src/dllmain.cpp
        src/exports.cpp
        src/Component.cpp
        src/Component.h
        src/ws_drv.cpp
        src/ws_drv.h
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
        include/server_response.h
        src/server_response.cpp
        )

if (ANDROID)
    list(APPEND SOURCES
            src/jnienv.cpp
            src/jnienv.h)
endif ()

add_library(${TARGET} SHARED
        ${SOURCES})

#target_compile_definitions(${TARGET} PRIVATE
#        UNICODE
#        _UNICODE)

if (CASE_INSENSITIVE)
    target_compile_definitions(${TARGET} PRIVATE CASE_INSENSITIVE)
endif ()

if (OUT_PARAMS)
    target_compile_definitions(${TARGET} PRIVATE OUT_PARAMS)
endif ()

target_include_directories(${TARGET} PRIVATE
        include
        ../arcirk/include
        ../ws_client/include)

if (WIN32 AND NOT MSVC)
    message(FATAL_ERROR "Must be compiled with MSVC on Windows")
endif ()

if (WIN32)
    if (STATIC_CRT)
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif ()
    target_compile_definitions(${TARGET} PRIVATE
            _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif ()

if (UNIX)
    if (TARGET_ARCH STREQUAL "x86")
        set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    endif ()
endif ()

if (ANDROID)
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ${CMAKE_STRIP} ${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif ()
endif (

)

if (UNIX)
    find_package(Boost REQUIRED COMPONENTS regex filesystem)
else()
    find_package(Boost REQUIRED COMPONENTS regex)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(ws_drv PRIVATE
        ${Boost_LIBRARIES}
        ${NMOS_CPP_SYSTEM_LIBS})
#        arcirk
#        ws_client)
#        #ws_client)