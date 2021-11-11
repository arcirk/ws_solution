QT += quick

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../qt_models/project/src/qjsontablemodel.cpp \
        ../../../qt_models/project/src/qproxymodel.cpp \
        main.cpp \
        src/messagelistmodel.cpp \
        src/selectedusersmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    ../../../qt_models/project/include/qjsontablemodel.h \
    ../../../qt_models/project/include/qproxymodel.h \
    include/messagelistmodel.h \
    include/selectedusersmodel.h

windows:LIBS += -L"F:/lib/vcpkg/installed/x64-windows/lib"
windows:INCLUDEPATH += F:/lib/vcpkg/installed/x64-windows/include

QT += quickcontrols2

#LIBS += -L"/home/arcady/src/ws_solution/qt_models/project/cmake-build-debug"

INCLUDEPATH += "/home/arcady/src/ws_solution/qt_models/project/include"
