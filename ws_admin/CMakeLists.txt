cmake_minimum_required(VERSION 3.20)
project(ws_admin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(CMAKE_PREFIX_PATH "/home/arcady/Qt/6.2.1/gcc_64/lib/cmake")
if(WIN32)
     add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
     add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS
#        Core
#        Gui
#        Widgets
#        Network
#        REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)


set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ws_admin.rc")

set(PROJECT_SOURCES
   main.cpp
   resources.qrc
   ../chared/clientsettings.h
   ../chared/qmlwebsocket.h
   ../chared/serveresponse.h
   ../chared/clientsettings.cpp
   ../chared/qmlwebsocket.cpp
   ../chared/serveresponse.cpp
   ../chared/qwebdav.h
   ../chared/qwebdav.cpp
   mainwindow.cpp
   mainwindow.h
   mainwindow.ui
   include/optionsdialog.h
   src/optionsdialog.cpp
   form/optionsdialog.ui
   include/groupdialog.h
   src/groupdialog.cpp
   form/groupdialog.ui
   include/userdialog.h
   src/userdialog.cpp
   form/userdialog.ui
   include/directoryinfobasedialog.h
   src/directoryinfobasedialog.cpp
   form/directoryinfobasedialog.ui
   include/aboutdialog.h
   src/aboutdialog.cpp
   form/aboutdialog.ui
   include/selectgroupdialog.h
   src/selectgroupdialog.cpp
   form/selectgroupdialog.ui
        ../ws_webdav/main.cpp ../ws_webdav/lib/include/bwebdav.h ../ws_webdav/lib/src/bwebdav.cpp)

if(WIN32)
     add_executable(ws_admin WIN32
             ${PROJECT_SOURCES}
             ${APP_ICON_RESOURCE_WINDOWS})
else()
     add_executable(ws_admin
             ${PROJECT_SOURCES})
endif()

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

link_directories(
        "../ws_client/cmake-build-debug"
)
include_directories("../ws_client/include")
#include_directories("../qt_models/project/include")
include_directories("../chared")

#link_directories(${CMAKE_BINARY_DIR}/lib)



target_link_libraries(ws_admin PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        ws_client
        )

#if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(ws_admin)
#endif()
