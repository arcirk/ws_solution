QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/arcirk.cpp \
    src/arcirk_ip.cpp \
    src/arcirk_json.cpp \
    src/arcirk_message.cpp \
    src/arcirk_settings.cpp \
    src/arcirk_static.cpp

HEADERS += \
    include/arcirk.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

windows:INCLUDEPATH += $(BOOST_INCLUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include
windows:LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib
Boost_USE_STATIC_LIBS = ON
windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS


