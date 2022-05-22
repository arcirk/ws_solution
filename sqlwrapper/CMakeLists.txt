cmake_minimum_required(VERSION 3.14)

project(sqlwrapper LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DSQLWRAPPER_LIBRARY)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(/MTd)
else()
    add_definitions(/MT)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (WIN32)
    set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
    set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
    set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    ON)
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)
find_package(Boost REQUIRED COMPONENTS regex)

add_library(sqlwrapper SHARED
        sqlwrapper_global.h
        sqlwrapper.h
        sqlwrapper.cpp
        sqlinterface.h
        sqlinterface.cpp
        arcirk.h
        arcirk.cpp
        )

include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(sqlwrapper PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
#        ${Boost_LIBRARIES}
        )

target_compile_definitions(sqlwrapper PRIVATE TESTCMAKE_LIBRARY)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()