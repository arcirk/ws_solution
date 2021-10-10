QT += core gui quickwidgets qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    activeroomsmodel.cpp \
    appsettings.cpp \
    main.cpp \
    mainwindow.cpp \
    popup.cpp \
    qmlmessage.cpp \
    qmlwebsocket.cpp \
    serveresponse.cpp \
    settingsdialog.cpp

HEADERS += \
    activeroomsmodel.h \
    appsettings.h \
    mainwindow.h \
    popup.h \
    qmlmessage.h \
    qmlwebsocket.h \
    serveresponse.h \
    settingsdialog.h \
    stdfx.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    AuthForm.qml \
    main.qml

RESOURCES += \
    res.qrc

unix:LIBS += -L"/usr/local/lib"
LIBS += -L"../../ws_client/cmake-build-debug"
unix:LIBS += -lboost_thread
LIBS += -lws_client
INCLUDEPATH += ../../ws_client/include

PRECOMPILED_HEADER = stdfx.h
