QT -= gui
QT += sql
QT += core

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../sqlwrapper/sqlinterface.cpp \
        http_session.cpp \
        listener.cpp \
        main.cpp \
        shared_state.cpp \
        sqlite/sqlite3.c \
        src/base.cpp \
        subscriber.cpp \
        websocket_session.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../sqlwrapper/sqlinterface.h \
    include/base.h \
    include/beast.hpp \
    include/http_session.hpp \
    include/listener.hpp \
    include/net.hpp \
    include/public_channels.h \
    include/shared_state.hpp \
    include/subscriber.h \
    include/websocket_session.hpp \
    sqlite/sqlite3.h

windows:INCLUDEPATH += $(BOOST_INCLDUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include
windows:LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib
Boost_USE_STATIC_LIBS = ON
windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS
windows:LIBS += -lbcrypt

DEFINES += USE_QT_CREATOR

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../arcirk/release/ -larcirk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../arcirk/debug/ -larcirk
else:unix: LIBS += -L$$OUT_PWD/../arcirk/ -larcirk

INCLUDEPATH += $$PWD/../arcirk/include
DEPENDPATH += $$PWD/../arcirk

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/release/libarcirk.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/debug/libarcirk.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/release/arcirk.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk/debug/arcirk.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../arcirk/libarcirk.a
