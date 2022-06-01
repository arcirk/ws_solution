cmake_minimum_required(VERSION 3.20)
project(cert_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_PREFIX_PATH "/home/arcady/Qt/6.2.1/gcc_64/lib/cmake")
#set(CMAKE_PREFIX_PATH C:/Qt/6.2.3/msvc2019_64/lib/cmake)

if(WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})



#find_package(QT NAMES Qt6 Qt5 COMPONENTS
#        Core
#        Gui
#        Widgets
#        Network
#        REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)


set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ws_admin.rc")

file(GLOB _SOURCES "${CMAKE_CURRENT_SOURCE_DIR}*.cpp *.h *.ui *.qrc")

set(PROJECT_SOURCES
        main.${_SOURCES})

if(WIN32)
    add_executable(ws_admin WIN32
            ${PROJECT_SOURCES}
            ${APP_ICON_RESOURCE_WINDOWS})
else()
    add_executable(ws_admin
            ${PROJECT_SOURCES})
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

link_directories(
        "../ws_client/cmake-build-debug"
)
include_directories("../ws_client/include")
#include_directories("../qt_models/project/include")
include_directories("../chared")

#link_directories(${CMAKE_BINARY_DIR}/lib)



target_link_libraries(ws_admin PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
        ws_client
        )

#if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(ws_admin)
#endif()
