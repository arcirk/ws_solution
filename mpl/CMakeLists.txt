cmake_minimum_required(VERSION 3.5)

project(MPL VERSION 0.1 LANGUAGES CXX)
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
add_subdirectory(singleapplication)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64_release/)
endif ()
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        main.cpp
        dialogmainwindow.cpp
        dialogmainwindow.h
        dialogmainwindow.ui
        mpl.qrc
        userprofile.cpp
        userprofile.h
        dialogselectprofile.cpp
        dialogselectprofile.h
        dialogselectprofile.ui
        dialogselectedrow.h
        dialogselectedrow.cpp
        dialogselectedrow.ui
        dialogclientoptions.cpp
        dialogclientoptions.h
        dialogclientoptions.ui
        dialogconnection.cpp
        dialogconnection.h
        dialogconnection.ui
        mplsettings.cpp
        mplsettings.h
        profilemanager.cpp
        profilemanager.h
        ../../chared/clientsettings.h
        ../../chared/qmlwebsocket.h
        ../../chared/serveresponse.h
        ../../chared/clientsettings.cpp
        ../../chared/qmlwebsocket.cpp
        ../../chared/serveresponse.cpp
        ../../chared/httpservice.cpp
        ../../chared/httpservice.h
        ../../sqlwrapper/sqlinterface.cpp
        ../../sqlwrapper/sqlinterface.h
        ../../sqlwrapper/sqlqueryinterface.cpp
        ../../sqlwrapper/sqlqueryinterface.h
        )

if (WIN32)

endif ()

#set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/logevent.rc")
find_package(QT NAMES Qt6 Qt5 COMPONENTS  Sql Network Core Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql Network Core Widgets REQUIRED)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    if(WIN32)
        qt_add_executable(MPL WIN32
                MANUAL_FINALIZATION
                ${PROJECT_SOURCES}
                ${APP_ICON_RESOURCE_WINDOWS}
                )

    else()
        qt_add_executable(MPL
                MANUAL_FINALIZATION
                ${PROJECT_SOURCES}
                )
    endif()
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET untitled APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MPL SHARED
                ${PROJECT_SOURCES}
                )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MPL
                ${PROJECT_SOURCES}
                ${DATABASE_SOURCES}
                )
    endif()
endif()

target_link_libraries(MPL PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
        SingleApplication::SingleApplication)

#
set_target_properties(MPL PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPL)
endif()

#target_link_libraries(MPL SingleApplication::SingleApplication)