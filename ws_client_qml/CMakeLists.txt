cmake_minimum_required(VERSION 3.20)
project(ws_client_qml)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/cmake_debug/)
else()
    SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/cmake_release/)
endif()
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_definitions(-DQT_QML_CLIENT_APP)
#
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

set(PROJECT_SOURCES
    qml/ChatBox.qml
    qml/IconPane.qml
    qml/ItemGroupPage.qml
    main.cpp
    qml/main.qml
    qml/MainForm.qml
    qml/MessageBox.qml
    qml/MessageDialog.qml
    src/messagelistmodel.cpp
    include/messagelistmodel.h
    qml.qrc
    qml/RoundPane.qml
    scripts/scripts.js
    src/selectedusersmodel.cpp
    include/selectedusersmodel.h
    qml/SmaileBox.qml
    qml/UsersBox.qml
    qml/UsersGroup.qml
    ../chared/clientsettings.h
    ../chared/serveresponse.h
    ../chared/qmlwebsocket.h
    ../chared/clientsettings.cpp
    ../chared/serveresponse.cpp
    ../chared/qmlwebsocket.cpp
    src/userinfo.cpp
    include/userinfo.h
    qml/UserInfoDialog.qml
    ../chared/qwebdav.h
    ../chared/qwebdav.cpp)



if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES DEBUG)
        add_executable(ws_client_qml
                ${PROJECT_SOURCES})
    else()
        add_executable(ws_client_qml WIN32
                ${PROJECT_SOURCES})
    endif()
else()
    add_executable(ws_client_qml
        ${PROJECT_SOURCES})
endif()

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        QuickControls2
        Network
        REQUIRED)


include_directories("../ws_client/include")
include_directories("../qt_models/project/include")
include_directories("../chared")

link_directories(
        "../ws_client/cmake-build-debug"
        "../qt_models/project/cmake-build-debug"
)

target_link_libraries(ws_client_qml PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        Qt6::QuickControls2
        Qt6::Network
        ws_client
        qjsonmodel
        )



#IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
#    message("debug mode")
#ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(ws_client_qml)
#endif()

