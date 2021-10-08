#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlApplicationEngine>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QQmlContext>
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qClient = new QmlWebSocket(this);

    ui->chatWidget->engine()->rootContext()->setContextProperty("wsClient", qClient);

    popUp = new PopUp();

    connect(qClient, SIGNAL(display_error(QString, QString)), this, SLOT(on_display_error(QString, QString)));

}

MainWindow::~MainWindow()
{
    delete qClient;
    delete ui;
}

void MainWindow::on_display_error(const QString& what, const QString& err)
{
    popUp->setPopupText(what + ": " + err);
    popUp->show();
}


void MainWindow::on_mnuSettings_triggered()
{
    SettingsDialog * dlg = new SettingsDialog(this, qClient->get_settings());
    dlg->setModal(true);
    dlg->exec();
    if (dlg->isAccepted()) {
        qClient->get_settings()->save_settings();
    }
}

