#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    QPushButton button("Hello world!", nullptr);
//    button.resize(200, 100);
//    button.show();
//    return QApplication::exec();

    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return a.exec();

}
