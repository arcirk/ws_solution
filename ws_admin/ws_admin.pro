QT += core gui
QT += widgets
QT += network
QT += sql

CONFIG += c++17

SOURCES += \
        ../chared/qjsontablemodel.cpp \
        ../sqlwrapper/sqlinterface.cpp \
        ../sqlwrapper/sqlqueryinterface.cpp \
        dialogselectinlist.cpp \
        httpservice.cpp \
        main.cpp \
        ../chared/clientsettings.cpp \
        ../chared/qmlwebsocket.cpp \
        ../chared/serveresponse.cpp \
        ../chared/qwebdav.cpp \
        mainwindow.cpp \
        src/optionsdialog.cpp \
        src/groupdialog.cpp \
        src/userdialog.cpp \
        src/directoryinfobasedialog.cpp \
        src/aboutdialog.cpp \
        src/selectgroupdialog.cpp

HEADERS += \
    ../chared/clientsettings.h \
    ../chared/qjsontablemodel.h \
    ../chared/qmlwebsocket.h \
    ../chared/qwebdav.h \
    ../chared/serveresponse.h \
    ../sqlwrapper/sqlinterface.h \
    ../sqlwrapper/sqlqueryinterface.h \
    dialogselectinlist.h \
    httpservice.h \
    include/optionsdialog.h \
    include/groupdialog.h \
    include/userdialog.h \
    include/directoryinfobasedialog.h \
    include/aboutdialog.h \
    include/selectgroupdialog.h \
    mainwindow.h

FORMS += \
    dialogselectinlist.ui \
    mainwindow.ui \
    form/aboutdialog.ui \
    form/directoryinfobasedialog.ui \
    form/groupdialog.ui \
    form/optionsdialog.ui \
    form/selectgroupdialog.ui \
    form/userdialog.ui

RESOURCES += resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ws_client/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ws_client/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../ws_client/ -lws_client

unix:LIBS += -lboost_thread
unix:LIBS += -lboost_filesystem

INCLUDEPATH += $$PWD/../ws_client/include
DEPENDPATH += $$PWD/../ws_client
INCLUDEPATH += $$PWD/../chared
INCLUDEPATH += $$PWD/../sqlwrapper

windows:INCLUDEPATH += $(BOOST_INCLDUE)

CONFIG(debug, debug|release): DEFINES +=  DEBUG_MODE

INCLUDEPATH += $$PWD/../ws_client
DEPENDPATH += $$PWD/../ws_client

DEFINES += USE_QT_CREATOR
windows:DEFINES += _WINDOWS
