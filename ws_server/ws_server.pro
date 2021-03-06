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
        ../sqlwrapper/sqlqueryinterface.cpp \
        http_session.cpp \
        listener.cpp \
        main.cpp \
        shared_state.cpp \
        src/base.cpp \
        subscriber.cpp \
        websocket_session.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../sqlwrapper/sqlinterface.h \
    ../sqlwrapper/sqlqueryinterface.h \
    include/base.h \
    include/beast.hpp \
    include/http_session.hpp \
    include/listener.hpp \
    include/net.hpp \
    include/public_channels.h \
    include/shared_state.hpp \
    include/subscriber.h \
    include/websocket_session.hpp

windows:INCLUDEPATH += $(BOOST_INCLDUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include
windows:LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib
Boost_USE_STATIC_LIBS = ON
windows:DEFINES += _WINDOWS
windows:DEFINES += _CRT_SECURE_NO_WARNINGS
windows:LIBS += -lbcrypt
unix:LIBS += -L/home/arcady/lib/vcpkg/installed/x64-linux/lib
unix:LIBS += -lboost_system
unix:LIBS += -lboost_filesystem

DEFINES += USE_QT_CREATOR

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../arcirk-Release/release/ -larcirk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../arcirk-Debug/debug/ -larcirk
else:unix: LIBS += -L$$OUT_PWD/../arcirk/ -larcirk

INCLUDEPATH += $$PWD/../arcirk/include
DEPENDPATH += $$PWD/../arcirk

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk-Release/release/libarcirk.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk-Debug/debug/libarcirk.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk-Release/release/arcirk.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arcirk-Debug/debug/arcirk.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../arcirk/libarcirk.a

DISTFILES += \
    ws_server.ico

RC_ICONS = ws_server.ico
