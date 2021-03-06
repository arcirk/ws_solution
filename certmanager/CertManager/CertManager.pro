QT += core gui sql
QT += core5compat
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../chared/clientsettings.cpp \
    ../../chared/qjsontablemodel.cpp \
    ../../chared/qmlwebsocket.cpp \
    ../../chared/qproxymodel.cpp \
    ../../chared/qwebdav.cpp \
    ../../chared/serveresponse.cpp \
    ../../sqlwrapper/sqlinterface.cpp \
    ../../sqlwrapper/sqlqueryinterface.cpp \
    ../../winreg/WinReg.cpp \
    certificate.cpp \
    comanndlineparser.cpp \
    commandline.cpp \
    databasemanager.cpp \
    dialogabout.cpp \
    dialogclientoptions.cpp \
    dialogcomputer.cpp \
    dialogconnection.cpp \
    dialogcontainerinfo.cpp \
    dialogcontainername.cpp \
    dialogmessage.cpp \
    dialogoptions.cpp \
    dialogselectdevice.cpp \
    dialogselectedrow.cpp \
    dialogselectfromdatabase.cpp \
    dialogselectinlist.cpp \
    dialogserveruser.cpp \
    dialogterminaloptions.cpp \
    keyscontainer.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    tabledelegate.cpp \
    user.cpp

HEADERS += \
    ../../chared/clientsettings.h \
    ../../chared/qjsontablemodel.h \
    ../../chared/qmlwebsocket.h \
    ../../chared/qproxymodel.h \
    ../../chared/qwebdav.h \
    ../../chared/serveresponse.h \
    ../../sqlwrapper/sqlinterface.h \
    ../../sqlwrapper/sqlqueryinterface.h \
    ../../winreg/WinReg.hpp \
    certificate.h \
    comanndlineparser.h \
    commandline.h \
    converter.h \
    databasemanager.h \
    dialogabout.h \
    dialogclientoptions.h \
    dialogcomputer.h \
    dialogconnection.h \
    dialogcontainerinfo.h \
    dialogcontainername.h \
    dialogmessage.h \
    dialogoptions.h \
    dialogselectdevice.h \
    dialogselectedrow.h \
    dialogselectfromdatabase.h \
    dialogselectinlist.h \
    dialogserveruser.h \
    dialogterminaloptions.h \
    keyscontainer.h \
    mainwindow.h \
    settings.h \
    tabledelegate.h \
    user.h

FORMS += \
    dialogabout.ui \
    dialogclientoptions.ui \
    dialogcomputer.ui \
    dialogconnection.ui \
    dialogcontainerinfo.ui \
    dialogcontainername.ui \
    dialogmessage.ui \
    dialogoptions.ui \
    dialogselectdevice.ui \
    dialogselectedrow.ui \
    dialogselectfromdatabase.ui \
    dialogselectinlist.ui \
    dialogserveruser.ui \
    dialogterminaloptions.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    certmaster.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ws_client-Release/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ws_client-Debug/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../../ws_client/ -lws_client

unix:LIBS += -lboost_filesystem
unix:LIBS += -lboost_thread

INCLUDEPATH += $$PWD/../../ws_client/include
DEPENDPATH += $$PWD/../../ws_client

INCLUDEPATH += $$PWD/../../chared
INCLUDEPATH += $$PWD/../../sqlwrapper
INCLUDEPATH += $$PWD/../../winreg

DEFINES += USE_QT_CREATOR
DEFINES += QT_CERT_MANAGER

windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS

RC_ICONS = CertManager.ico

DISTFILES += \
    CertManager.ico

win32 {
    QMAKE_TARGET_PRODUCT = "Certificate manager"
    QMAKE_TARGET_DESCRIPTION = "Managing certificates in an organization"
}

VERSION = 1.0.0
QMAKE_TARGET_COPYRIGHT = (c)(arcirk) Arcady Borisoglebsky
