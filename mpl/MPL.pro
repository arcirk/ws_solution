QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogmainwindow.cpp \
    dialogselectedrow.cpp \
    dialogselectprofile.cpp \
    main.cpp \
    profilemanager.cpp \
    userprofile.cpp
#    singleapplication/singleapplication.cpp \
#    singleapplication/singleapplication_p.cpp \

HEADERS += \
    dialogmainwindow.h \
    dialogselectedrow.h \
    dialogselectprofile.h \
    profilemanager.h \
    userprofile.h
#    singleapplication/singleapplication_p.h \
#    singleapplication/singleapplication.h \

FORMS += \
    dialogmainwindow.ui \
    dialogselectedrow.ui \
    dialogselectprofile.ui

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
