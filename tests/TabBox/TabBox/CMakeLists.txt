cmake_minimum_required(VERSION 3.20)
project(chat_qml_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(chat_qml_test
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
        qml/UsersGroup.qml)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        QuickControls2
        REQUIRED)

include_directories("../../../qt_models/project/include")

target_link_libraries(chat_qml_test
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        Qt6::QuickControls2
        qjsonmodel
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