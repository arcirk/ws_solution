cmake_minimum_required(VERSION 3.20)
project(ws_chat_qt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ws_chat_qt
        appsettings.cpp
        appsettings.h
        AuthPage.qml
        ChatPage.qml
        main.cpp
        main.qml
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        NextChatPage.qml
        popup.cpp
        popup.h
        qmlmessage.cpp
        qmlmessage.h
        qmlwebsocket.cpp
        qmlwebsocket.h
        res.qrc
        serveresponse.cpp
        serveresponse.h
        settingsdialog.cpp
        settingsdialog.h
        settingsdialog.ui
        stdfx.h
        )

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        REQUIRED)

include_directories("../../ws_client/include")
link_directories(../../ws_client/cmake-build-debug)

target_link_libraries(ws_chat_qt
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        ws_client
        )

target_precompile_headers(ws_chat_qt
        PUBLIC
        stdfx.h)

add_custom_command(
        TARGET ws_chat_qt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Gui>
        $<TARGET_FILE:Qt6::Network>
        $<TARGET_FILE:Qt6::Qml>
        $<TARGET_FILE:Qt6::Quick>
        $<TARGET_FILE:Qt6::QmlModels>
        $<TARGET_FILE:Qt6::QuickWidgets>
        $<TARGET_FILE_DIR:ws_client>)
