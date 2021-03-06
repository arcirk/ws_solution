cmake_minimum_required(VERSION 3.21)

project(ws_agent VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_PREFIX_PATH "C:/Qt/6.2.3/msvc2019_64/winreg/cmake")

#SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if(WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_definitions(-DQT_AGENT_APP)

link_directories("../../ws_client/cmake-build-debug")
include_directories("../../chared")
include_directories("../../ws_client/include")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ws_agent.rc")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ws_agent.qrc
        ../../chared/qmlwebsocket.cpp
        ../../chared/qmlwebsocket.h
        ../../chared/clientsettings.cpp
        ../../chared/serveresponse.cpp
        ../../chared/serveresponse.h
        ../../chared/clientsettings.h
        ../../chared/qwebdav.cpp
        ../../chared/qwebdav.h
        include/clientapp.h
        src/clientapp.cpp)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_add_executable(ws_agent WIN32
#        MANUAL_FINALIZATION
#        ${PROJECT_SOURCES}
#    )
    if(WIN32)
#        qt_add_executable(ws_agent WIN32
#                MANUAL_FINALIZATION
#                ${PROJECT_SOURCES})
        if(CMAKE_BUILD_TYPE MATCHES DEBUG)
            qt_add_executable(ws_agent
                    MANUAL_FINALIZATION
                    ${PROJECT_SOURCES}
                    ${APP_ICON_RESOURCE_WINDOWS})
        else()
            qt_add_executable(ws_agent WIN32
                    MANUAL_FINALIZATION
                    ${PROJECT_SOURCES}
                    ${APP_ICON_RESOURCE_WINDOWS})
        endif()
    else()
        qt_add_executable(ws_agent
                MANUAL_FINALIZATION
                ${PROJECT_SOURCES})
    endif()
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ws_agent APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ws_agent SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
#        add_executable(ws_agent
#            ${PROJECT_SOURCES}
#
#        )
        if(WIN32)
            add_executable(ws_agent WIN32
                    ${PROJECT_SOURCES})
        else()
            add_executable(ws_agent
                    ${PROJECT_SOURCES})
        endif()
    endif()
endif()

target_link_libraries(ws_agent PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        ws_client)

set_target_properties(ws_agent PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER arcirk.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ws_agent)
endif()


