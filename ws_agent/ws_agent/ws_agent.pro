QT       += core gui quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/clientsettings.cpp \
    src/qmlwebsocket.cpp \
    src/serveresponse.cpp

HEADERS += \
    mainwindow.h \
    include/clientsettings.h \
    include/qmlwebsocket.h \
    include/serveresponse.h

FORMS +=  \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.json

unix:LIBS += -L"/usr/local/lib"

LIBS += -L"../../ws_client/cmake-build-debug"
windows:LIBS += -L"C:\src\ws_solution\ws_client\cmake-build-debug"

INCLUDEPATH += ../../ws_client/include

#windows:LIBS += -L"F:/lib/vcpkg/installed/x64-windows/lib"
#windows:INCLUDEPATH += F:/lib/vcpkg/installed/x64-windows/include

unix:LIBS += -lboost_thread
LIBS += -lws_client

#PRECOMPILED_HEADER = stdfx.h

RESOURCES += \
    ws_agent.qrc