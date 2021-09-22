QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    ../../ws_client/src/ws_client.cpp \
#    ../../ws_client/src/ws_session.cpp \
    appSettings.cpp \
    main.cpp \
    mainwindow.cpp \
    optionsdlg.cpp \
    serveresponse.cpp

HEADERS += \
#    ../../ws_client/include/beast.h \
#    ../../ws_client/include/global.h \
#    ../../ws_client/include/net.h \
#    ../../ws_client/include/ws_client.h \
#    ../../ws_client/include/ws_session.h \
    AppSettings.h \
    global.h \
    mainwindow.h \
    optionsdlg.h \
    serveresponse.h \
    stdfx.h

FORMS += \
    mainwindow.ui \
    optionsdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.json

LIBS += -L"/usr/local/lib"

LIBS += -L"../../ws_client/cmake-build-debug"

INCLUDEPATH += ../../ws_client/include

#LIBS += F:/lib/vcpkg/installed/x64-windows/lib
#INCLUDEPATH += F:/lib/vcpkg/installed/x64-windows/include

LIBS += -lboost_thread
LIBS += -lws_client

PRECOMPILED_HEADER = stdfx.h
