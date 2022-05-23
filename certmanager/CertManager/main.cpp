#include "mainwindow.h"
#include <QStyleFactory>
#include <QApplication>
//#include <Windows.h>
//#include <locale.h>

int main(int argc, char *argv[])
{
    //setlocale(LC_ALL, "Russian");

    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();
    return a.exec();
}
