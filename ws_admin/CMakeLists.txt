cmake_minimum_required(VERSION 3.21)
project(ws_admin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/arcady/Qt/6.2.1/gcc_64/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(ws_admin
        main.cpp
        resources.qrc)

target_link_libraries(ws_admin
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

