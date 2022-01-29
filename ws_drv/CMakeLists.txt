cmake_minimum_required(VERSION 3.21)
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
        src/ws_drv.h)

file(GLOB ARCIRK_SOURCES ../arcirk/src/*.cpp)

add_library(${TARGET} SHARED
        ${SOURCES}
        ${ARCIRK_SOURCES})

set(ARCIRK_INCLUDE ../arcirk/include)
set(ARCIRK_LIB ../arcirk/cmake-build-debug)

target_compile_definitions(${TARGET} PRIVATE UNICODE _UNICODE)
target_include_directories(${TARGET} PRIVATE
        include
        ${ARCIRK_INCLUDE})

set (LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})
set (CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

if (UNIX)
    set(MySuffix2 "")
else(UNIX)
    if (NOT MSVC)
        message(FATAL_ERROR "Must be compiled with MSVC on Windows")
    endif(NOT MSVC)
    set(MyPrefix "lib")
    set(MySuffix1 "Win")
endif(UNIX)

set_target_properties( ${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${MyPrefix}${PROJECT_NAME}${MySuffix1}${MySuffix2}
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
    target_link_libraries(${PROJECT_NAME} -static-libstdc++)
else(UNIX)
    add_definitions(/MT)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    target_compile_definitions(${TARGET} PRIVATE _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif(UNIX)


#link_directories(${ARCIRK_LIB})

if (MSVC)
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE})
#            ../wdc/include
#    )
    link_libraries( bcrypt.lib )
else()
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE})
#            ../wdc/include
#    )
    find_package(Boost COMPONENTS regex filesystem system thread REQUIRED)
    link_libraries(Boost::filesystem Boost::system Boost::thread)
    #link_libraries(libwdc)
endif()