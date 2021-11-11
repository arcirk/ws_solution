cmake_minimum_required(VERSION 3.20)
project(ws_gclient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ws_gclient
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
        include/clientsettings.h
        include/serveresponse.h
        include/qmlwebsocket.h
        src/clientsettings.cpp
        src/serveresponse.cpp
        src/qmlwebsocket.cpp
        include/stdfx.h)
#        include/usersmodel.h
#        src/usersmodel.cpp
#        include/usersproxymodel.h
#        src/usersproxymodel.cpp)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        QuickWidgets
        QmlModels
        QuickControls2
        REQUIRED)

include_directories("../../ws_client/include")
include_directories("../../qt_models/project/include")

target_link_libraries(ws_gclient
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickWidgets
        Qt6::QmlModels
        Qt6::QuickControls2
        ws_client
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
