QT       += core gui quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../../chared/clientsettings.cpp \
    ../../chared/qmlwebsocket.cpp \
    ../../chared/serveresponse.cpp \
    ../../chared/qwebdav.cpp \
    src/clientapp.cpp

HEADERS += \
    include/clientapp.h \
    mainwindow.h \
    ../../chared/clientsettings.h \
    ../../chared/qmlwebsocket.h \
    ../../chared/qwebdav.h \
    ../../chared/serveresponse.h

FORMS +=  \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.json

unix:LIBS += -L"/usr/local/lib"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ws_client-Release/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ws_client-Debug/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../../ws_client/ -lws_client

INCLUDEPATH += $$PWD/../../ws_client/include
DEPENDPATH += $$PWD/../../ws_client
INCLUDEPATH += $$PWD/../../chared

unix:LIBS += -lboost_thread
unix:LIBS += -lboost_filesystem


#PRECOMPILED_HEADER = stdfx.h

RESOURCES += \
    ws_agent.qrc


DEFINES += USE_QT_CREATOR
win32:DEFINES += _WINDOWS
