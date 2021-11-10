cmake_minimum_required(VERSION 3.20)
project(qjsonmodel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(qjsonmodel include/qjsontablemodel.h src/qjsontablemodel.cpp include/qproxymodel.h src/qproxymodel.cpp)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

target_link_libraries(qjsonmodel
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        )