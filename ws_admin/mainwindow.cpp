#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/optionsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings.init();

    lblStatusService = new QLabel(this);
    statusBar()->addWidget(lblStatusService);
    lblStatusSocket = new QLabel(this);
    statusBar()->addWidget(lblStatusSocket);

    isServiceStarted = settings.statusService();
    if(isServiceStarted)
        lblStatusService->setText("Сервис запущен на локальном комьютере.");
    else
        lblStatusService->setText("Сервис не запущен.");


    lblStatusSocket->setText("Статус подключения: Не активное");

    client = new bWebSocket(this);

    connect(client, SIGNAL(connectedStatusChanged(bool)), this, SLOT(connectedStatusChanged(bool)));
    connect(client, SIGNAL(displayError(QString,QString)), this, SLOT(displayError(QString,QString)));

    if(settings.AutoConnect){
        client->open(settings.RootUser, "");
    }

    QAction *pQCmd = ui->dockSrvObjects->toggleViewAction();
    ui->mnuView->addAction(pQCmd);

    listChildServerObjects = ui->listChildSrvObjects;
    treeServerObjects = ui->treeSrvObjects;

}

QString MainWindow::serverView(){
    return settings.ServerName + " [" + settings.ServerHost + ":" + QString::number(settings.ServerPort) + "]";
}

void MainWindow::fillTree(bool started){

   treeServerObjects->clear();

   auto * root = new QTreeWidgetItem();
   root-> setText (0, "root");
   root->setText(1, "root");
   treeServerObjects->addTopLevelItem(root);

   auto * itemServer = new QTreeWidgetItem();
   itemServer->setText(0, serverView());
   itemServer->setToolTip(0, serverView());
   itemServer->setText(1, "ServerName");
   if(started){
       auto * itemActiveUsers = new QTreeWidgetItem();
       itemActiveUsers->setText(0,"Активные подключения");
       itemActiveUsers->setToolTip(0, "Активные подключения");
       itemActiveUsers->setText(1, "ActiveUsers");
       itemServer->addChild(itemActiveUsers);
       auto * itemUsers = new QTreeWidgetItem();
       itemUsers->setText(0,"Пользователи");
       itemUsers->setToolTip(0, "Пользователи");
       itemUsers->setText(1, "Users");
       itemServer->addChild(itemUsers);
   }else
   {
       auto * itemErr = new QTreeWidgetItem();
       itemErr->setText(0,"Сервер не доступен");
       itemErr->setText(1,"error");
       itemErr->setToolTip(0, "Сервер не доступен");
       itemServer->addChild(itemErr);
   }
   root->addChild(itemServer);

   treeServerObjects->expandAll();
   treeServerObjects->setCurrentItem(
       treeServerObjects->topLevelItem(0),    // item
       0,                          // column
       QItemSelectionModel::Select // command
   );
    visibilityСontrol(false, false);
}

void MainWindow::visibilityСontrol(bool visible, bool isSessions) {
    ui->panelGroupUsers->setVisible(visible);
    ui->btnAddUser->setVisible(visible);
    ui->btnDeleteUser->setVisible(visible);
    ui->btnEditUser->setVisible(visible);
    ui->btnToGroup->setVisible(visible);
    ui->btnViewMode->setVisible(visible);
    ui->btnKillSession->setVisible(isSessions);
    ui->btnSendMessage->setVisible(isSessions);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}


void MainWindow::on_mnuOptions_triggered()
{

    auto optDlg = new OptionsDialog(this, settings);
    optDlg->setModal(true);
    optDlg->exec();

    if(optDlg->result() == QDialog::Accepted){
        settings = optDlg->getSettings();
        if(settings.pwdEdit){
            settings.Hash = QString::fromStdString(IClient::get_hash(settings.RootUser.toStdString(), settings.password.toStdString()));
        }
        settings.pwdEdit = false;
        settings.password = "";
        settings.save_settings();
    }

    delete optDlg;

}


void MainWindow::on_mnuInstallService_triggered()
{
    if(isServiceStarted){
        qDebug() << "Сервис уже запущен!";
        return;
    }

    bool bOk;
    QString str = QInputDialog::getText( 0,
                                         "Пароль root",
                                         "Пароль:",
                                         QLineEdit::Password,
                                         "",
                                         &bOk
    );
    if (!bOk) {
        return;
    }

    bool result = settings.installService(str);
    if(!result){
        qDebug() << "Ошибка установки службы!";
    }else
        qDebug() << "Служба успешно установлена!";

    isServiceStarted = settings.statusService();
    if(isServiceStarted)
        lblStatusService->setText("Сервис запущен на локальном комьютере.");
    else
        lblStatusService->setText("Сервис не запущен.");
}


void MainWindow::on_mnuStart_triggered()
{
    if(isServiceStarted){
        qDebug() << "Сервис уже запущен!";
        return;
    }

    if (settings.RunAsService){

        bool bOk;
        QString str = QInputDialog::getText( 0,
                                             "Пароль root",
                                             "Пароль:",
                                             QLineEdit::Password,
                                             "",
                                             &bOk
        );
        if (!bOk) {
            return;
        }

        bool result = settings.startService(str);
        if(!result){
            qDebug() << "Ошибка запуска службы!";
        }else
            qDebug() << "Служба успешно запущена!";
    }

    isServiceStarted = settings.statusService();
    if(isServiceStarted)
        lblStatusService->setText("Сервис запущен на локальном комьютере.");
    else
        lblStatusService->setText("Сервис не запущен.");

    if(isServiceStarted){
        if(settings.AutoConnect && !client->isStarted())
            client->open(settings.RootUser, "");
   }
}


void MainWindow::on_mnuStop_triggered()
{
    if(!isServiceStarted){
        qDebug() << "Сервис не запущен!";
        return;
    }

    if (settings.RunAsService){

        bool bOk;
        QString str = QInputDialog::getText( 0,
                                             "Пароль root",
                                             "Пароль:",
                                             QLineEdit::Password,
                                             "",
                                             &bOk
        );
        if (!bOk) {
            return;
        }

        bool result = settings.stopService(str);
        if(!result){
            qDebug() << "Ошибка остановки службы!";
        }else
            qDebug() << "Служба успешно остановлена!";
    }
    isServiceStarted = settings.statusService();
    if(isServiceStarted)
        lblStatusService->setText("Сервис запущен на локальном комьютере.");
    else
        lblStatusService->setText("Сервис не запущен.");
}


void MainWindow::on_mnuStatusService_triggered()
{

    if(settings.statusService()){
        qDebug() << "Сервис запущен!";
    }
    else
        qDebug() << "Сервис не запущен!";


}

void MainWindow::connectedStatusChanged(bool status) {
    //qDebug() << "connection status: " << status;
    QString wsStatus = status ? "Активное" : "Не активное";
    lblStatusSocket->setText("Статус подключения: " + wsStatus);
    fillTree(status);
}

void MainWindow::displayError(const QString &what, const QString &err)
{
    qDebug() << "error: " << what << ":" << err;
}


void MainWindow::on_mnuConnect_triggered()
{
    if (client->isStarted()) {
        return;
    }

    client->updateSettings();

    client->open(settings.RootUser, "");
}


void MainWindow::on_mnuDisconnect_triggered()
{
    if (client->isStarted()) {
        client->close();
    }
}

