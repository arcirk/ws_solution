QT       += core gui quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appSettings.cpp \
    chatboxdialog.cpp \
    directoryinfobasedialog.cpp \
    groupdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    optionsdlg.cpp \
    popup.cpp \
    selectgroupdialog.cpp \
    serveresponse.cpp \
    userdialog.cpp

HEADERS += \
    AppSettings.h \
    chatboxdialog.h \
    directoryinfobasedialog.h \
    global.h \
    groupdialog.h \
    mainwindow.h \
    optionsdlg.h \
    popup.h \
    selectgroupdialog.h \
    serveresponse.h \
    stdfx.h \
    userdialog.h

FORMS += \
    chatboxdialog.ui \
    directoryinfobasedialog.ui \
    groupdialog.ui \
    mainwindow.ui \
    optionsdlg.ui \
    selectgroupdialog.ui \
    userdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.json

unix:LIBS += -L"/usr/local/lib"

LIBS += -L"../../ws_client/cmake-build-debug"
windows:LIBS += -L"C:/src/GitHub/ws_solution/ws_client/cmake-build-debug"

INCLUDEPATH += ../../ws_client/include

windows:LIBS += -L"F:/lib/vcpkg/installed/x64-windows/lib"
windows:INCLUDEPATH += F:/lib/vcpkg/installed/x64-windows/include

unix:LIBS += -lboost_thread
LIBS += -lws_client

PRECOMPILED_HEADER = stdfx.h

RESOURCES += \
    resourse.qrc

QT += quick qml
