cmake_minimum_required(VERSION 3.20)
project(ws_admin_qt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ws_admin_qt
        appSettings.cpp
        AppSettings.h
        global.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        optionsdlg.cpp
        optionsdlg.h
        optionsdlg.ui
        serveresponse.cpp
        serveresponse.h
        stdfx.h
        ws_admin_qt.pro
        popup.cpp
        popup.h
        resourse.qrc
        directoryinfobasedialog.cpp
        directoryinfobasedialog.h
        directoryinfobasedialog.ui
        userdialog.cpp
        userdialog.h
        userdialog.ui
        groupdialog.cpp
        groupdialog.h
        groupdialog.ui
        selectgroupdialog.cpp
        selectgroupdialog.h
        selectgroupdialog.ui
        Chat.qml
        chatboxdialog.h
        chatboxdialog.cpp
        chatboxdialog.ui)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        OpenGL
        QmlModels
        REQUIRED)

include_directories("../../ws_client/include")
link_directories(../../ws_client/cmake-build-debug)
#target_link_libraries(ws_admin_qt PUBLIC ws_client)

target_link_libraries(ws_admin_qt
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::OpenGL
        Qt6::QmlModels
        ws_client
        )



target_precompile_headers(ws_admin_qt
        PUBLIC
        stdfx.h)

add_custom_command(
        TARGET ws_admin_qt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Gui>
        $<TARGET_FILE:Qt6::Network>
        $<TARGET_FILE:Qt6::Qml>
        $<TARGET_FILE:Qt6::Quick>
        $<TARGET_FILE:Qt6::OpenGL>
        $<TARGET_FILE:Qt6::QmlModels>
        $<TARGET_FILE:Qt6::QuickWidgets>
        $<TARGET_FILE_DIR:ws_admin_qt>
        $<TARGET_FILE_DIR:ws_client>)
