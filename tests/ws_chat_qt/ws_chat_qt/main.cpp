#include "mainwindow.h"
#include <QApplication>
#include <QtQml>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qmlRegisterType<QmlWebSocket>("QmlWebSocket", 1,0, "QmlWebSocket");

    MainWindow w;
    w.show();

    return a.exec();
}
