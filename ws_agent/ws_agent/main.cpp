#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(ws_agent);

    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("ws_agent"),
                              QObject::tr("Не обноружено ни одной панели задач! "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);
    QIcon appIcon(":/img/images/app_icon.png");
    QApplication::setWindowIcon(appIcon);
    MainWindow w;
    w.show();
    return a.exec();
}
