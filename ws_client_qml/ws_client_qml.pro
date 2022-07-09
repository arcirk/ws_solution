QT += quick
QT += widgets
QT += network
QT += quickcontrols2

SOURCES += \
        ../chared/clientsettings.cpp \
        ../chared/qjsontablemodel.cpp \
        ../chared/qmlwebsocket.cpp \
        ../chared/qproxymodel.cpp \
        ../chared/qwebdav.cpp \
        ../chared/serveresponse.cpp \
        main.cpp \
        src/messagelistmodel.cpp \
        src/selectedusersmodel.cpp \
        src/userinfo.cpp

#resources.files = main.qml
#resources.prefix = /$${TARGET}
#RESOURCES += resources
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ws_client-Release/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ws_client-Debug/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../ws_client/ -lws_client

unix:LIBS += -lboost_thread
unix:LIBS += -lboost_filesystem

INCLUDEPATH += $$PWD/../ws_client/include
DEPENDPATH += $$PWD/../ws_client
INCLUDEPATH += $$PWD/../chared
#windows:INCLUDEPATH += C:/lib/vcpkg/vcpkg/installed/x64-windows/include
windows:INCLUDEPATH += $(BOOST_INCLDUE)

DEFINES += QT_QML_CLIENT_APP

CONFIG(debug, debug|release): DEFINES +=  DEBUG_MODE

HEADERS += \
    ../chared/clientsettings.h \
    ../chared/qjsontablemodel.h \
    ../chared/qmlwebsocket.h \
    ../chared/qproxymodel.h \
    ../chared/qwebdav.h \
    ../chared/serveresponse.h \
    include/messagelistmodel.h \
    include/selectedusersmodel.h \
    include/userinfo.h

windows:DEFINES += _WINDOWS
