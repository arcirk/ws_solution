
#include "dialogmainwindow.h"
#include <QStyleFactory>
#include <QApplication>
#include "singleapplication/singleapplication.h"

int main(int argc, char *argv[])
{
    SingleApplication app( argc, argv );
    app.setQuitOnLastWindowClosed(false);

    QIcon appIcon(":/img/Firefox.ico");
    qApp->setStyle(QStyleFactory::create("Fusion"));

    DialogMainWindow w;
    w.show();
    return app.exec();
}
