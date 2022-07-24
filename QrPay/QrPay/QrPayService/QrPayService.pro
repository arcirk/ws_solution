TEMPLATE = lib
TARGET = service
CONFIG += dll
QT += core androidextras

SOURCES += \
    service_main.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
