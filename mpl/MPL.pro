QT       += core gui
QT       += network sql
QT       += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../certmanager/CertManager/certificate.cpp \
    ../certmanager/CertManager/commandline.cpp \
    ../certmanager/CertManager/keyscontainer.cpp \
    ../certmanager/CertManager/settings.cpp \
    ../certmanager/CertManager/tabledelegate.cpp \
    ../certmanager/CertManager/user.cpp \
    ../chared/clientsettings.cpp \
    ../chared/httpservice.cpp \
    ../chared/qjsontablemodel.cpp \
    ../chared/qmlwebsocket.cpp \
    ../chared/qwebdav.cpp \
    ../chared/serveresponse.cpp \
    ../sqlwrapper/sqlinterface.cpp \
    ../sqlwrapper/sqlqueryinterface.cpp \
    ../winreg/WinReg.cpp \
    dialogclientoptions.cpp \
    dialogconnection.cpp \
    dialogmainwindow.cpp \
    dialogselectedrow.cpp \
    dialogselectprofile.cpp \
    dialogsettings.cpp \
    main.cpp \
    mplsettings.cpp \
    profilemanager.cpp \
    userprofile.cpp
#    singleapplication/singleapplication.cpp \
#    singleapplication/singleapplication_p.cpp \

HEADERS += \
    ../certmanager/CertManager/certificate.h \
    ../certmanager/CertManager/commandline.h \
    ../certmanager/CertManager/converter.h \
    ../certmanager/CertManager/keyscontainer.h \
    ../certmanager/CertManager/settings.h \
    ../certmanager/CertManager/tabledelegate.h \
    ../certmanager/CertManager/user.h \
    ../chared/clientsettings.h \
    ../chared/httpservice.h \
    ../chared/qjsontablemodel.h \
    ../chared/qmlwebsocket.h \
    ../chared/qwebdav.h \
    ../chared/serveresponse.h \
    ../sqlwrapper/sqlinterface.h \
    ../sqlwrapper/sqlqueryinterface.h \
    ../winreg/WinReg.hpp \
    dialogclientoptions.h \
    dialogconnection.h \
    dialogmainwindow.h \
    dialogselectedrow.h \
    dialogselectprofile.h \
    dialogsettings.h \
    mplsettings.h \
    profilemanager.h \
    userprofile.h
#    singleapplication/singleapplication_p.h \
#    singleapplication/singleapplication.h \

FORMS += \
    dialogclientoptions.ui \
    dialogconnection.ui \
    dialogmainwindow.ui \
    dialogselectedrow.ui \
    dialogselectprofile.ui \
    dialogsettings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mpl.qrc

include(singleapplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

#CONFIG(debug, debug|release) {
    TARGET = mpl
#} else {
#    TARGET = mpl_static
#}

DISTFILES += \
    mpl_static.exe.manifest

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ws_client-Release/release/ -lws_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ws_client-Debug/debug/ -lws_client
else:unix: LIBS += -L$$OUT_PWD/../ws_client/ -lws_client

unix:LIBS += -lboost_filesystem
unix:LIBS += -lboost_thread

INCLUDEPATH += $$PWD/../ws_client/include
DEPENDPATH += $$PWD/../ws_client
INCLUDEPATH += $$PWD/../chared
INCLUDEPATH += $$PWD/../sqlwrapper
INCLUDEPATH += $$PWD/../certmanager/CertManager
INCLUDEPATH += $$PWD/../winreg

DEFINES += USE_QT_CREATOR
DEFINES += QT_MPL_CLIENT

windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS

windows:INCLUDEPATH += $(BOOST_INCLDUE)
windows:LIBS += -L$(BOOST_LIB)
Boost_USE_STATIC_LIBS = ON
