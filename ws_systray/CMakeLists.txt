cmake_minimum_required(VERSION 3.20)
project(ws_systray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        QuickControls2
        REQUIRED)

add_executable(ws_systray main.cpp window.h window.cpp systray.qrc)

target_link_libraries(ws_systray
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        Qt6::QuickControls2
        )

