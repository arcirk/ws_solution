cmake_minimum_required(VERSION 3.10)
project(ws_drv)

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_drv)

option(CASE_INSENSITIVE "Case insensitive method names" OFF)
option(STATIC_CRT "Static CRT linkage" OFF)
option(OUT_PARAMS "Support output parameters" OFF)

list(APPEND SOURCES
        src/addin.def
        src/stdafx.h
        src/dllmain.cpp
        src/exports.cpp
        src/Component.cpp
        src/Component.h
        src/ws_drv.cpp
        src/ws_drv.h
        ../ws_client/include/beast.h
        ../ws_client/include/net.h
        ../ws_client/include/ws_session.h
        ../ws_client/include/ws_client.h
        ../ws_client/src/ws_session.cpp
        ../ws_client/src/ws_client.cpp
        )

if (ANDROID)
    list(APPEND SOURCES
            src/jnienv.cpp
            src/jnienv.h)
endif ()

add_library(${TARGET} SHARED
        ${SOURCES})

target_compile_definitions(${TARGET} PRIVATE
        UNICODE
        _UNICODE)

if (CASE_INSENSITIVE)
    target_compile_definitions(${TARGET} PRIVATE CASE_INSENSITIVE)
endif ()

if (OUT_PARAMS)
    target_compile_definitions(${TARGET} PRIVATE OUT_PARAMS)
endif ()

target_include_directories(${TARGET} PRIVATE
        include)

#if (WIN32 AND NOT MSVC)
#    message(FATAL_ERROR "Must be compiled with MSVC on Windows")
#endif ()

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
endif ()

set_target_properties( ${TARGET} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD 17
        )

if (UNIX)
    if (TARGET_PLATFORM_32)
        set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    else(TARGET_PLATFORM_32)
        set(CMAKE_C_FLAGS "-m64 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-m64 ${CMAKE_CXX_FLAGS}")
    endif ()
    target_link_libraries(${TARGET} -static-libstdc++)
else(UNIX)
    add_definitions(/MT)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    target_compile_definitions(${TARGET} PRIVATE _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif(UNIX)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

find_package(Boost REQUIRED COMPONENTS regex)
find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries( ${TARGET}
        ${Boost_LIBRARIES}
        Boost::thread
        ${NMOS_CPP_SYSTEM_LIBS}
)


include_directories("../ws_client/include")

ADD_CUSTOM_COMMAND(TARGET ${TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/ws_drv)

ADD_CUSTOM_COMMAND(
        TARGET ${TARGET}
        POST_BUILD
        COMMENT "Copying lib..."
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_CURRENT_SOURCE_DIR}/bin/ws_drv
)

ADD_CUSTOM_COMMAND(
        TARGET ${TARGET}
        POST_BUILD
        COMMENT "Copying resources..."
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/res/* ${CMAKE_CURRENT_SOURCE_DIR}/bin/ws_drv
)

if (UNIX)
    ADD_CUSTOM_COMMAND(TARGET ${TARGET}
            POST_BUILD
            COMMENT "Archive layout..."
            COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/install.sh
            )
else()
    ADD_CUSTOM_COMMAND(TARGET ${TARGET}
            POST_BUILD
            COMMENT "Archive layout..."
            COMMAND 7z a -tzip -mx5 -r0  ../bin/layout.zip ../bin/ws_drv/*
            )
endif ()


