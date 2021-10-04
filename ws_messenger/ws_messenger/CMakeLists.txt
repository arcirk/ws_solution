cmake_minimum_required(VERSION 3.19)
project(ws_messenger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ws_messenger
        AuthForm.qml
        AuthFormForm.ui.qml
        AuthorizationForm.qml
        GroupItems.qml
        ListMessages.qml
        main.cpp
        main.qml
        qml.qrc
        )

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        REQUIRED)

target_link_libraries(ws_messenger
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        )