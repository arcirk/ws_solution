QT += quick
QT += widgets
QT += network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../qt_models/project/src/qjsontablemodel.cpp \
        ../../qt_models/project/src/qproxymodel.cpp \
        main.cpp \
        src/messagelistmodel.cpp \
        src/selectedusersmodel.cpp \
        ../../chared/clientsettings.cpp \
        ../../chared/serveresponse.cpp \
        ../../chared/qmlwebsocket.cpp \
        src/webdav.cpp \
        userinfo.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    ../../qt_models/project/include/qjsontablemodel.h \
    ../../qt_models/project/include/qproxymodel.h \
    include/messagelistmodel.h \
    include/selectedusersmodel.h \
    ../../chared/clientsettings.h \
    ../../chared/serveresponse.h \
    ../../chared/qmlwebsocket.h \
    include/webdav.h \
    userinfo.h

#pch.h /
#windows:LIBS += -L"F:/lib/vcpkg/installed/x64-windows/lib"
#windows:INCLUDEPATH += F:/lib/vcpkg/installed/x64-windows/include

QT += quickcontrols2

#PRECOMPILED_HEADER = pch.h

LIBS += -L"../../ws_client/cmake-build-debug"
#LIBS += -L"../../qt_models/project/cmake-build-debug"
windows:LIBS += -L"C:\src\ws_solution\ws_client\cmake-build-debugg"

INCLUDEPATH += ../../ws_client/include
INCLUDEPATH += ../../qt_models/project/include
INCLUDEPATH += ../../chared

unix:LIBS += -lboost_thread
LIBS += -lws_client

DEFINES += QT_QML_CLIENT_APP
