QT -= gui

TEMPLATE = lib
DEFINES += WS_CLIENT_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/iws_client.cpp \
    src/ws_client.cpp \
    src/ws_session.cpp

HEADERS += \
    include/beast.h \
    include/iws_client_global.h \
    include/iws_client.h \
    include/net.h \
    include/stdafx.h \
    include/ws_client.h \
    include/ws_session.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../arcirk/release/ -larcirk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../arcirk/debug/ -larcirk
else:unix: LIBS += -L$$OUT_PWD/../arcirk/ -larcirk

INCLUDEPATH += $$PWD/../arcirk
DEPENDPATH += $$PWD/../arcirk

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/release/libarcirk.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/debug/libarcirk.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/release/arcirk.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/debug/arcirk.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../arcirk/libarcirk.a

windows:INCLUDEPATH += $(BOOST_INCLDUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include
windows:LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib
Boost_USE_STATIC_LIBS = ON
windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS
windows:LIBS += -lbcrypt
