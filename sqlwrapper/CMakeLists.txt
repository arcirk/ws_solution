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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

add_library(sqlwrapper SHARED
        sqlwrapper_global.h
        sqlwrapper.h
        sqlwrapper.cpp
        sqlinterface.h
        sqlinterface.cpp
        )

target_link_libraries(sqlwrapper PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        )

target_compile_definitions(sqlwrapper PRIVATE TESTCMAKE_LIBRARY)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()