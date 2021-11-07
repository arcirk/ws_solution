QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS       = window.h
SOURCES       = main.cpp \
                window.cpp
RESOURCES     = systray.qrc

FORMS         = window.ui

CONFIG += c++17

#requires(qtConfig(combobox))

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
INSTALLS += target
