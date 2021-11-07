cmake_minimum_required(VERSION 3.20)
project(ws_client_qml)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ws_client_qml
        main.cpp
        main.qml
        qml.qrc
        src/qmlwebsocket.cpp
        src/serveresponse.cpp
        src/clientsettings.cpp
        include/qmlwebsocket.h
        include/serveresponse.h
        include/clientsettings.h
        stdfx.h
        )

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        QuickControls2
        REQUIRED)

include_directories("../ws_client/include")
link_directories(../ws_client/cmake-build-debug)

target_link_libraries(ws_client_qml
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        Qt6::QuickControls2
        ws_client
        )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_custom_command(
            TARGET chat_qml_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE:Qt6::Qml>
            $<TARGET_FILE:Qt6::Quick>
            $<TARGET_FILE:Qt6::QmlModels>
            $<TARGET_FILE:Qt6::QuickWidgets>
            $<TARGET_FILE:Qt6::QuickControls2>)
endif()