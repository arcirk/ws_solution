#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    QIcon appIcon(":/img/ws_admin.ico");
    QApplication::setWindowIcon(appIcon);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return a.exec();

}
