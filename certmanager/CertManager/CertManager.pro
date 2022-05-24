QT += core gui sql
QT += core5compat
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../chared/clientsettings.cpp \
    ../../chared/qmlwebsocket.cpp \
    ../../chared/qwebdav.cpp \
    ../../chared/serveresponse.cpp \
    dialogconnection.cpp \
    dialogoptions.cpp \
    dialogselectdevice.cpp \
    dialogselectinlist.cpp \
    keyscontainer.cpp \
    main.cpp \
    mainwindow.cpp \
    registry.cpp \
    settings.cpp \
    user.cpp

HEADERS += \
    ../../chared/clientsettings.h \
    ../../chared/qmlwebsocket.h \
    ../../chared/qwebdav.h \
    ../../chared/serveresponse.h \
    dialogconnection.h \
    dialogoptions.h \
    dialogselectdevice.h \
    dialogselectinlist.h \
    keyscontainer.h \
    mainwindow.h \
    registry.h \
    settings.h \
    user.h

FORMS += \
    dialogconnection.ui \
    dialogoptions.ui \
    dialogselectdevice.ui \
    dialogselectinlist.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    certmaster.qrc

#INCLUDEPATH += C:/src/CertManager/lib/ws_client/include
#DEPENDPATH += C:/src/CertManager/lib/ws_client

#LIBS += -LC:/src/CertManager/lib/ws_client -lws_client

#windows:DEFINES -= UNICODE
#windows:DEFINES += _MBCS

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ws_client/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ws_client/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../../ws_client/ -lws_client

INCLUDEPATH += $$PWD/../../ws_client/include
DEPENDPATH += $$PWD/../../ws_client
INCLUDEPATH += $$PWD/../../chared

DEFINES += USE_QT_CREATOR
win32:DEFINES += _WINDOWS