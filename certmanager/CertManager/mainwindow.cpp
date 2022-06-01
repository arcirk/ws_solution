#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnection.h"
#include <QMessageBox>
#include <QSqlError>
#include "registry.h"
#include <QSqlQuery>
#include <QFileDialog>
#include "dialogselectinlist.h"
#include <QPixmap>
#include "dialogoptions.h"
#include <clientsettings.h>
#include <QSqlQueryModel>
#include <dialogcomputer.h>
#include <QSqlError>
#include <qjsontablemodel.h>
#include <QStringConverter>
#include <QInputDialog>
#include "dialogterminaloptions.h"
#include <QTextOption>
#include "dialogselectfromdatabase.h"
#include "serveresponse.h"

#ifdef _WINDOWS
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
    #pragma warning(disable:4100)
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << __FUNCTION__;

    ui->setupUi(this);

    initCsptest(); // init CriptoPro

    formControl();

    _sett = new Settings(this);

    createTerminal();

    createConnectionsObjects();

    setWsConnectedSignals();

    if(_sett->launch_mode() == mixed){
        if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
             connectToDatabase(_sett, _sett->pwd());
    }

    connectToWsServer();

    //

//
//
//

//    createWsObject();

//







//    ui->horizontalLayout->addStretch();
//    ui->toolBarActiveUsers->addStretch();





//

//    if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
//        onReconnect(_sett, _sett->pwd());

//    setWindowTitle("Менеджер сертификатов");



}

void MainWindow::toolBarSetVisible(QWidget * bar, bool value){
    qDebug() << __FUNCTION__;
    bar->setVisible(value);
}

void MainWindow::createWsObject()
{
    qDebug() << __FUNCTION__;
    m_client = new bWebSocket(this, "conf_qt_cert_manager.json");
//    m_client->options()[bConfFieldsWrapper::AppName] = "qt_cert_manager";
//    m_client->options()[bConfFieldsWrapper::User] = "admin";
//    QString hash = bWebSocket::generateHash("admin", "admin");
//    m_client->options()[bConfFieldsWrapper::Hash] = hash;
//    m_client->options().save();
}

void MainWindow::setWsConnectedSignals()
{
    qDebug() << __FUNCTION__;
    connect(m_client, &bWebSocket::connectionSuccess, this, &MainWindow::onConnectionSuccess);
    connect(m_client, &bWebSocket::closeConnection, this, &MainWindow::onCloseConnection);
    connect(m_client, &bWebSocket::connectedStatusChanged, this, &MainWindow::onConnectedStatusChanged);
    connect(m_client, &bWebSocket::clientJoinEx, this, &MainWindow::onClientJoinEx);
    connect(m_client, &bWebSocket::clientLeave, this, &MainWindow::onClientLeave);
    connect(m_client, &bWebSocket::displayError, this, &MainWindow::onDisplayError);
    connect(m_client, &bWebSocket::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_client, &bWebSocket::getActiveUsers, this, &MainWindow::onGetActiveUsers);
}

void MainWindow::createTerminal()
{
    currentUser = new CertUser(this);
    QString curentHost = QSysInfo::machineHostName();
    currentUser->setDomain(curentHost);

    terminal = new CommandLine(this, false, _sett->charset());
    ui->txtTerminal->setWordWrapMode(QTextOption::NoWrap);
    QPalette pal = ui->txtTerminal->palette();
    pal.setBrush(QPalette::Base, Qt::black);
    ui->txtTerminal->setAutoFillBackground(true);
    ui->txtTerminal->setPalette(pal);
    ui->txtTerminal->setTextColor(QColor(0,255,0));
    terminal->setMethod(_sett->method());

    connect(terminal, &CommandLine::output, this, &MainWindow::onOutputCommandLine);
    connect(terminal, &CommandLine::endParse, this, &MainWindow::onParseCommand);

    terminal->start();
    if(isUseCsptest)
        terminal->send(QString("cd %1\n").arg(_cprocsp_dir), cmdCommand::unknown);

#ifdef _WINDOWS
        std::string envUSER = "username";
        if(_sett->charset() != "CP866")
            terminal->setChcp();
       //terminal->send("echo %username%\n", CommandLine::cmdCommand::echoUserName);// ; exit\n

        QByteArray data(std::getenv(envUSER.c_str()));
        QString uname = QString::fromLocal8Bit(data);
        currentUser->setName(uname);
#else
        std::string envUSER = "USER";
        QString cryptoProDir = "/opt/cprocsp/bin/amd64/";
        terminal->send("echo $USER\n", 1); //CommandLine::cmdCommand::echoUserName);// ; exit\n
#endif



}

void MainWindow::initToolBars()
{
    qDebug() << __FUNCTION__;
    toolBar.append(ui->btnAdd);
    toolBar.append(ui->btnEdit);
    toolBar.append(ui->btnDelete);
    toolBar.append(ui->btnImportFromDatabase);
    toolBar.append(ui->btnInstallToUser);
    toolBar.append(ui->btnToDatabase);
    toolBarActiveUsers.append(ui->btnUserToDatabase);
    toolBarActiveUsers.append(ui->btnCompToDatabase);
    toolBarSetVisible(ui->wToolBarAU, false);
    toolBarSetVisible(ui->wToolbarContainers, false);
}

void MainWindow::setKeysToRegistry()
{
    qDebug() << __FUNCTION__;
    auto dlg = new DialogSelectDevice(this, "База данных");
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){

        if(dlg->currentSelection() == 0){

            QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
            if(dir != ""){
                QFile file(dir + QDir::separator() + "name.key");
                if(file.open(QIODevice::ReadOnly)){
                    QString data = QString::fromLocal8Bit(file.readAll());
                    //qDebug() << data;
                    QRegularExpression ex("0/\u0016-");
                    if(ex.match(data).hasMatch()){
                        data.replace("0/\u0016-", "");
                        auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));
                        if(result == QMessageBox::Yes){
                            QString error;
                            bool result = Registry::importKeysFromLocalCatalog(currentUser, dir, data, error);
                            if(result)
                                QMessageBox::information(this, "Успех", "Контейнер успешно импортирован в реестр!");
                            else{
                                if(!error.isEmpty())
                                    QMessageBox::critical(this, "Ошибка", "Ошибка импорта контейнера!");
                            }
                        }
                    }else
                       QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }else{
                    QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }
            }
        }else if(dlg->currentSelection() == 1){

        }else if(dlg->currentSelection() == 2){

        }
    }
}

bool MainWindow::isDbOpen()
{
    qDebug() << __FUNCTION__;
    QString status;

    bool result = db->isOpen();

    if(db->isOpen()){
        status = "SQL Server: " + _sett->server() + "  ";
    }

    if(m_client->isStarted()){
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
    }

    infoBar->setText(status);

    return result;
}

void MainWindow::formControl()
{
    qDebug() << __FUNCTION__;
    initToolBars();
    createTree();
    infoBar = new QLabel(this);
    ui->statusbar->addWidget(infoBar);
    infoBar->setText("Не подключен.");
}

void MainWindow::initCsptest()
{

    isUseCsptest = false;

#ifdef _WINDOWS
   _cprocsp_exe = "C:/Program Files (x86)/Crypto Pro/CSP/csptest.exe";
   _cprocsp_dir = "C:/Program Files (x86)/Crypto Pro/CSP/";
#else
    _cprocsp_exe = "/opt/cprocsp/bin/amd64/cprocsp";
    _cprocsp_dir = "/opt/cprocsp/bin/amd64/"
#endif

    QFile csptest(_cprocsp_exe);
    if(!csptest.exists())
        qCritical() << __FUNCTION__ << "Утилита csptest не найдена!";
    else{
        //qDebug() << __FUNCTION__ << "Утилита csptest установлена на компьютере!";
        isUseCsptest = true;
    }
    //https://redos.red-soft.ru/base/other-soft/szi/certs-cryptopro/

}

void MainWindow::csptestGetContainers(const QString &result)
{
    qDebug() << __FUNCTION__;

    if(result.isEmpty())
        return;

    QStringList keys = result.split("\n");
    auto tableView = ui->tableView;
    tableView->setModel(nullptr);
    auto table = new QStandardItemModel(this);
    table->setColumnCount(2);
    //table->setRowCount(keys.size());
    QStringList cols = {"","Наименование"};
    table->setHorizontalHeaderLabels(cols);
    auto colDb = table->horizontalHeaderItem(0);
    colDb->setIcon(QIcon(":/img/externalDataTable.png"));
    //int i = 0;
    foreach(const QString& key, keys){
        if(key.isEmpty())
            continue;
        table->setRowCount(table->rowCount() + 1);
        auto itemTable = new QStandardItem(key);
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
        table->setItem(table->rowCount() - 1, 1, itemTable);
        if(isContainerExists(key)){
            auto itemIco = new QStandardItem();
            itemIco->setIcon(QIcon(":/img/checked.png"));
            table->setItem(table->rowCount() - 1, 0, itemIco);

//            QLabel *lbl_item = new QLabel();
//            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/checked.png")));// *ui->my_label->pixmap());
//            lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//            tableView->setCellWidget(i, 0, lbl_item);
        }
        //i++;
    }
    tableView->setModel(table);
    tableView->resizeColumnsToContents();


}

QTreeWidgetItem *MainWindow::addTreeNode(const QString &text, const QVariant &key, const QString &imagePath)
{
    qDebug() << __FUNCTION__;
    auto * node = new QTreeWidgetItem();
    node-> setText (0, text);
    node->setData(0, Qt::UserRole, key);
    node->setIcon(0, QIcon(imagePath));
    return node;
}

QTreeWidgetItem *MainWindow::findTreeItem(const QString &key)
{
    auto tree = ui->treeWidget;
    auto root = tree->topLevelItem(0);
    return findTreeItem(key, root);
}

QTreeWidgetItem * MainWindow::findTreeItem(const QString& key, QTreeWidgetItem* parent)
{
    int i;

    if (!parent)
        return nullptr;

    if(parent->childCount()>0){
        for(i=0;i<parent->childCount();i++){
            if(parent->child(i)!=0)
            {
                qDebug() << parent->child(i)->data(0, Qt::UserRole).toString();

                if (parent->child(i)->data(0, Qt::UserRole).toString() == key)
                    return parent->child(i);
                else
                    findTreeItem(key, parent->child(i));
            }

        }
    }

   return nullptr;
}

void MainWindow::onOutputCommandLine(const QString &data, int command)
{
    //qDebug() << __FUNCTION__;
    //qDebug() << __FUNCTION__ << qPrintable(data);
    ui->txtTerminal->setText(ui->txtTerminal->toPlainText() + data);
    qDebug() << __FUNCTION__ << "commant: " << command;
    terminal->parseCommand(data, command);
}

MainWindow::~MainWindow()
{
    terminal->stop();

    if(m_client)
        delete m_client;
    delete ui;
}

void MainWindow::createTree()
{
    qDebug() << __FUNCTION__;
    auto tree = ui->treeWidget;   
    //tree->setColumnCount(1);
    tree->setHeaderHidden(true);
//    tree->setColumnHidden(1, true);

//    auto * root = new QTreeWidgetItem();
//    auto * rootId = new QTreeWidgetItem();
//    root-> setText (0, "root");
//    rootId-> setText (0, "root");
//    root->setIcon(0, QIcon(":/img/system_config_root.ico"));


    auto root = addTreeNode("root", "root", ":/img/system_config_root.ico");
    tree->addTopLevelItem(root);
    auto curr_user = addTreeNode("Текущий пользователь", "currentUser", ":/img/userOptions.ico");
    root->addChild(curr_user);
    auto server = addTreeNode("База", "SqlServer", ":/img/sqlServer.png");
    root->addChild(server);
    auto ws = addTreeNode("Сервер взаимодействия", "WsServer", ":/img/socket_16_only.ico");
    root->addChild(ws);

////    QString usrName = currentUser->name();

//    auto * curr_user = new QTreeWidgetItem();
////    if(usrName.isEmpty())
//        curr_user->setText(0, "Текущий пользователь");
////    else
////       curr_user->setText(0, QString("Текущий пользователь (%1)").arg(usrName));
////    curr_user->setToolTip(0, QString("Текущий пользователь (%1)").arg(usrName));
//    curr_user->setIcon(0, QIcon(":/img/userOptions.ico"));
//    root->addChild(curr_user);

    //currentUser->setTreeItem(curr_user);

//    auto * allowed_keys = new QTreeWidgetItem();
//    allowed_keys->setText(0, "Доступные контейнеры");
//    //allowed_keys->setIcon(0, QIcon(":/img/registry.ico"));
//    curr_user->addChild(allowed_keys);

//    auto * currUserReg = new QTreeWidgetItem();
//    currUserReg->setText(0, "Реестр");
//    currUserReg->setIcon(0, QIcon(":/img/registry.ico"));
//    allowed_keys->addChild(currUserReg);

//    auto * currUserDevice = new QTreeWidgetItem();
//    currUserDevice->setText(0, "Устройства");
//    //currUserReg->setIcon(0, QIcon(":/img/registry.ico"));
//    allowed_keys->addChild(currUserDevice);

//    auto * server = new QTreeWidgetItem();
//    server->setText(0, "База");
//    server->setIcon(0, QIcon(":/img/sqlServer.png"));
//    root->addChild(server);

//    auto * containers = new QTreeWidgetItem();
//    containers->setText(0, "Контейнеры");
//    containers->setIcon(0, QIcon(":/img/key_password_lock_800.ico"));
//    server->addChild(containers);

//    auto * certs = new QTreeWidgetItem();
//    certs->setText(0, "Сертификаты");
//    certs->setIcon(0, QIcon(":/img/certificate.ico"));
//    server->addChild(certs);

//    auto * users = new QTreeWidgetItem();
//    users->setText(0, "Пользователи");
//    users->setIcon(0, QIcon(":/img/bootloader_users_person_people_6080.ico"));
//    server->addChild(users);

//    auto * comps = new QTreeWidgetItem();
//    comps->setText(0, "Компьютеры");
//    comps->setIcon(0, QIcon(":/img/computers.ico"));
//    root->addChild(comps);

//    auto * ws = new QTreeWidgetItem();
//    ws->setText(0, "Сервер взаимодействия");
//    ws->setIcon(0, QIcon(":/img/socket_16_only.ico"));
//    root->addChild(ws);

//    auto * activeUsers = new QTreeWidgetItem();
//    activeUsers->setText(0, "Активные пользователи");
//    activeUsers->setIcon(0, QIcon(":/img/activeUesers.png"));
//    ws->addChild(activeUsers);

    tree->expandAll();
}

void MainWindow::createRootList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);

    if(!isDbOpen())
        return;

    auto model = new QStandardItemModel(this);

    model->setColumnCount(1);
    model->setRowCount(3);

    QStringList cols = {"root"};
    model->setHorizontalHeaderLabels(cols);

    auto itemKeys = new QStandardItem("Контейнеры");
    itemKeys->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemKeys->setIcon(QIcon(":/img/key_password_lock_800.ico"));
    model->setItem(0, 0, itemKeys);
    auto itemCerts = new QStandardItem("Сертификаты");
    itemCerts->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemCerts->setIcon(QIcon(":/img/certificate.ico"));
    model->setItem(1, 0, itemCerts);
    auto itemUsers = new QStandardItem("Пользователи");
    itemUsers->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemUsers->setIcon(QIcon(":/img/bootloader_users_person_people_6080.ico"));
    model->setItem(2, 0, itemUsers);

    table->setModel(model);
    table->resizeColumnsToContents();

}

void MainWindow::loadContainersList()
{

    qDebug() << __FUNCTION__;
    auto tableView = ui->tableView;
    tableView->setModel(nullptr);

    if(!isDbOpen())
        return;

    auto table = new QStandardItemModel(this);
    table->setColumnCount(2);
    QStringList cols = {"Наименование", "id"};
    table->setHorizontalHeaderLabels(cols);


    QSqlQuery query("SELECT [Ref] , [FirstField] AS name FROM [arcirk].[dbo].[Containers]", db->getDatabase());
    int i = 0;
    while (query.next()) {
        table->setRowCount(table->rowCount()+ 1);
        int id = query.value(0).toInt();
        QString name = query.value(1).toString().trimmed();
        auto itemTable = new QStandardItem(name);
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
        table->setItem(i, 0, itemTable);
        auto itemId = new QStandardItem(QString::number(id));
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(i, 1, itemId);
        i++;
    }

    tableView->setModel(table);
    tableView->setColumnHidden(1, true);
    tableView->resizeColumnsToContents();
}

void MainWindow::LoadUsersList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);
    ui->btnAdd->setEnabled(true);

    if(!isDbOpen())
        return;

    QString result;
    QString query = "SELECT NULL AS isOnline\n"
            ",[FirstField] AS Имя\n"
            ",[Ref] AS Ссылка\n"
            ",[uuid] AS ID\n"
            ",[sid] AS SID\n"
            ",[host] AS Host\n"
            "FROM [dbo].[CertUsers]";

    auto model = new QSqlQueryModel(this);
    model->setQuery(query, db->getDatabase());
    ui->tableView->setModel(model);


    for (int i = 0; i < ui->tableView->model()->rowCount(); ++i) {
        QModelIndex index = ui->tableView->model()->index(i, 0);
        QModelIndex indexUuid = ui->tableView->model()->index(i, 3);
        QUuid uuid = QUuid::fromString(indexUuid.data().toString().trimmed());
         auto iter = m_actUsers.find(uuid);

         QLabel *lbl_item = new QLabel();
         if(iter != m_actUsers.end()){
             if(iter.value()->online()){
                 lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/online.png")));
             }else{
                 lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/ofline.png")));
             }

         }else{
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/ofline.png")));
         }
         lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
         ui->tableView->setIndexWidget(index, lbl_item);
     }

    ui->tableView->model()->setHeaderData(0, Qt::Orientation::Horizontal, "");
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::loadCertList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);
    ui->btnAdd->setEnabled(true);

    if(!isDbOpen())
        return;

    QString result;
    QString query = "SELECT [_id]\n"
                    ",[FirstField]\n"
                    ",[SecondField]\n"
                    ",[Ref]\n"
                    ",[privateKey]\n"
                    "FROM [dbo].[Certificates]";


    auto model = new QSqlQueryModel(this);
    model->setQuery(query, db->getDatabase());
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();

}

void MainWindow::createUsersList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);
}

void MainWindow::enableToolbar(bool value)
{
    qDebug() << __FUNCTION__;
    for (auto btn : toolBar) {
        btn->setEnabled(value);
    }
}

void MainWindow::loadItemChilds(QTreeWidgetItem *item)
{
    qDebug() << __FUNCTION__;
    auto tableView = ui->tableView;
    tableView->setModel(nullptr);
    auto table = new QStandardItemModel(this);
    table->setColumnCount(1);
    table->setRowCount(item->childCount());
    QStringList cols = {"root"};
    table->setHorizontalHeaderLabels(cols);
    for (int i = 0; i < item->childCount(); i++) {
        auto child = item->child(i);
        auto itemTable = new QStandardItem(child->text(0));
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
        table->setItem(i, 0, itemTable);
    }

    tableView->setModel(table);
    tableView->resizeColumnsToContents();
}

void MainWindow::loadItemSpecific(QTreeWidgetItem *item)
{
qDebug() << __FUNCTION__;
}

void MainWindow::loadKeysOnRegistry(CertUser *usr)
{
    qDebug() << __FUNCTION__;
//    QStringList keys = usr->containers();
//    auto tableView = ui->tableView;
//    tableView->setModel(nullptr);
//    auto table = new QStandardItemModel(this);
//    table->setColumnCount(2);
//    table->setRowCount(keys.size());
//    QStringList cols = {"","Наименование"};
//    table->setHorizontalHeaderLabels(cols);
//    auto colDb = table->horizontalHeaderItem(0);
//    colDb->setIcon(QIcon(":/img/externalDataTable.png"));
//    int i = 0;
//    foreach(const QString& key, keys){
//        auto itemTable = new QStandardItem(key);
//        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
//        table->setItem(i, 1, itemTable);
//        if(isContainerExists(key)){
//            auto itemIco = new QStandardItem();
//            itemIco->setIcon(QIcon(":/img/checked.png"));
//            table->setItem(i, 0, itemIco);

////            QLabel *lbl_item = new QLabel();
////            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/checked.png")));// *ui->my_label->pixmap());
////            lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
////            tableView->setCellWidget(i, 0, lbl_item);
//        }
//        i++;
//    }
//    tableView->setModel(table);
//    tableView->resizeColumnsToContents();

   terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", cmdCommand::csptestGetConteiners);
}

void MainWindow::loadOnlineUsers()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);

    if(!m_client->isStarted())
        return;

    auto model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setRowCount(m_actUsers.size() - 1);
    QStringList header = {"Имя", "Хост", "ID пользователя", "SID"};
    model->setHorizontalHeaderLabels(header);
    int row = 0;
    for(auto itr : m_actUsers){
        if(itr->online()){
            auto name = new QStandardItem(itr->name());
            model->setItem(row, 0, name);
            auto host = new QStandardItem(itr->domain());
            model->setItem(row, 1, host);
            auto uuid = new QStandardItem(itr->uuid().toString());
            model->setItem(row, 2, uuid);
            auto sid = new QStandardItem(itr->sid());
            model->setItem(row, 3, sid);
        }
    }
    table->setModel(model);
    table->resizeColumnsToContents();
}

void MainWindow::disableToolBar()
{
    qDebug() << __FUNCTION__;
    ui->btnAdd->setEnabled(false);
    ui->btnEdit->setEnabled(false);
    ui->btnDelete->setEnabled(false);
    ui->btnImportFromDatabase->setEnabled(false);
    ui->btnToDatabase->setEnabled(false);
}

bool MainWindow::isContainerExists(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(db->isOpen()){
        QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Containers] WHERE [FirstField] = '%1'").arg(name), db->getDatabase());
        while (query.next()) {
            return true;
        }
    }

    return false;
}

bool MainWindow::isCertUserExists(const QString &name, const QString& host)
{
    qDebug() << __FUNCTION__;
    if(db->isOpen()){
        QString _host;
        if(!host.isEmpty()){
            _host = QString(" AND [host] = '%1'").arg(host);
        }
        QString str = QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[CertUsers] WHERE [FirstField] = '%1'%2").arg(name, _host);
        //qDebug() << str;
        QSqlQuery query(str, db->getDatabase());
        while (query.next()) {
            return true;
        }
    }

    return false;
}

bool MainWindow::isHostExists(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(db->isOpen()){
        QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Servers] WHERE [FirstField] = '%1'").arg(name), db->getDatabase());
        while (query.next()) {
            return true;
        }
    }

    return false;
}

void MainWindow::userToDatabase(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(isCertUserExists(name))
    {
        QMessageBox::information(this, "Пользователь", "Пользователь уже есть на севрвере!");
    }
}

void MainWindow::loadCimputers()
{
    qDebug() << __FUNCTION__;
    ui->tableView->setModel(nullptr);
    auto queryModel = new QSqlQueryModel(this);
    queryModel->setQuery("SELECT [_id]\n"
                         ", NULL AS Сервер\n"
                         ",[FirstField] AS Наименование\n"
                         ",[SecondField] AS Представление\n"
                         ",[Ref] AS Идентификатор\n"
                         ",[ipadrr] AS Адрес\n"
                         ",[isServer]\n"
                         "FROM [dbo].[Servers]", db->getDatabase());

    ui->tableView->setModel(queryModel);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(4, true);
    ui->tableView->setColumnHidden(6, true);

    UpdateRowIcons();
    ui->tableView->model()->setHeaderData(1, Qt::Orientation::Horizontal, "");
    ui->tableView->resizeColumnsToContents();

    disableToolBar();
    ui->btnAdd->setEnabled(true);
    ui->btnEdit->setEnabled(true);
    ui->btnDelete->setEnabled(true);
}

void MainWindow::UpdateRowIcons(){
    qDebug() << __FUNCTION__;
   for (int i = 0; i < ui->tableView->model()->rowCount(); ++i) {
       QModelIndex index = ui->tableView->model()->index(i, 1);
       QLabel *lbl_item = new QLabel();
        if(ui->tableView->model()->index(i, 6).data().toInt() > 0){
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/server.ico")));// *ui->my_label->pixmap());
        }else
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/desktop.ico")));
        lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableView->setIndexWidget(index, lbl_item);
    }
}

void MainWindow::connectToWsServer()
{
    qDebug() << __FUNCTION__;

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
    }
    if(!m_client)
        return;

    if(m_client->isStarted())
        return;

    if(_sett->launch_mode() == mixed){
        QSqlQuery result("select [host], [port] from dbo.WSConf;", db->getDatabase());
        if(result.lastError().type() == QSqlError::NoError){
            while (result.next()){
                QString _host = result.value(0).toString().trimmed();
                int _port = result.value(1).toInt();
                m_client->setHost(_host);
                m_client->setPort(_port);
                m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
                break;
            }
        }
    }else{
        QString _host = m_client->options()[bConfFieldsWrapper::ServerHost].toString();
        int _port = m_client->options()[bConfFieldsWrapper::ServerPort].toInt();
        m_client->setHost(_host);
        m_client->setPort(_port);
        m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
    }


}

//bool MainWindow::insertSqlTableRow(const QString &table, QMap<QString, QVariant>& vals, const QString& ref)
//{

//    db.exec("USE arcirk;");

//    QString query = queryText(table, vals, sqlCommand::sqlInsert, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//bool MainWindow::updateSqlTableRow(const QString &table, QMap<QString, QVariant> vals, const QString& ref)
//{

//    QString query = queryText(table, vals, sqlCommand::sqlUpdate, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//bool MainWindow::deleteSqlTableRow(const QString &table, const QString& ref)
//{
//    QMap<QString, QVariant> vals;
//    QString query = queryText(table, vals, sqlCommand::sqlDelete, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//QString MainWindow::queryText(const QString& table, QMap<QString, QVariant>& values,
//                                sqlCommand command, const QString& not_ref)
//{
//    QString query;


//    if(command == sqlCommand::sqlInsert){

//        QString into = "\n(";
//        QString values_ = "\n(";
//        query = "INSERT INTO ";
//        query.append("dbo." + table);
//        for (auto iter = values.begin(); iter != values.end(); iter++) {
//            into.append(iter.key());
//            if (iter != --values.end())
//                into.append(",\n");

//            if (iter.value().typeId() == QMetaType::QString){
//                QString value = iter.value().toString();
//                values_.append("'" + value + "'");
//                if (iter != --values.end())
//                    values_.append(",\n");
//            }else if (iter.value().typeId() == QMetaType::Int){
//                int res = iter.value().toInt();
//                QString value = QString::number(res);
//                values_.append("'" + value + "'");
//                if (iter != --values.end())
//                    values_.append(",\n");
//            }
//        }
//        into.append("\n)");
//        values_.append("\n)");
//        query.append(into);
//        query.append("\nVALUES\n");
//        query.append(values_);

//        if(!not_ref.isEmpty()){
//            QString query_ = QString("IF NOT EXISTS \n"
//                                     "    (   SELECT  [Ref]\n"
//                                     "        FROM    dbo.%1 \n"
//                                     "        WHERE   [Ref]='%2' \n"
//                                     "    )\n"
//                                     "BEGIN\n").arg(table, not_ref);
//            query_.append(query);
//            query_.append("\nEND");
//            query = query_;
//        }

//        query.append(";");

//    }else if(command == sqlCommand::sqlUpdate){
//        query = "UPDATE dbo." + table;
//        QString _set = "\n SET ";
//        QString _where = QString("\n WHERE Ref = '%1'").arg(not_ref);
//        for (auto iter = values.begin(); iter != values.end(); iter++) {
//            _set.append(iter.key());
//            if (iter.value().typeId() == QMetaType::QString){
//                QString value = iter.value().toString();
//                _set.append(" = '" + value + "'");
//                if (iter != --values.end())
//                    _set.append(",\n");
//            }else if (iter.value().typeId() == QMetaType::Int){
//                int res = iter.value().toInt();
//                QString value = QString::number(res);
//                _set.append(" = '" + value + "'");
//                if (iter != --values.end())
//                    _set.append(",\n");
//            }

//        }
//        _set.append("\n");
//        _where.append("\n");
//        query.append(_set);
//        query.append(_where + ";");
//    }else if(command == sqlCommand::sqlDelete){
//        query = QString("DELETE FROM [dbo].[Servers]\n"
//                "WHERE Ref = '%1'").arg(not_ref);
//    }

//    return query;

//}

void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

    qDebug() << __FUNCTION__;
    if(item->childCount() > 0){
        loadItemChilds(item);
        disableToolBar();
        toolBarSetVisible(ui->wToolBarAU, false);
        toolBarSetVisible(ui->wToolBarMain, true);
        toolBarSetVisible(ui->wToolbarContainers, false);
    }else{
        QString itemText = item->text(0);
        if(itemText == "Реестр"){
            ui->btnAdd->setEnabled(true);

            if(item->parent()->text(0).compare("Текущий пользователь")){
                loadKeysOnRegistry(currentUser);
            }else{

            }
        }else if(itemText == "Контейнеры"){
            toolBarSetVisible(ui->wToolBarAU, false);
            toolBarSetVisible(ui->wToolBarMain, false);
            toolBarSetVisible(ui->wToolbarContainers, true);
            loadContainersList();
        }else if(itemText == "Пользователи"){
            toolBarSetVisible(ui->wToolBarAU, false);
            toolBarSetVisible(ui->wToolBarMain, true);
            toolBarSetVisible(ui->wToolbarContainers, false);
            LoadUsersList();
        }else if(itemText == "Компьютеры"){
            toolBarSetVisible(ui->wToolBarAU, false);
            toolBarSetVisible(ui->wToolBarMain, true);
            toolBarSetVisible(ui->wToolbarContainers, false);
            loadCimputers();
        }else if(itemText == "Активные пользователи"){
            toolBarSetVisible(ui->wToolBarAU, true);
            toolBarSetVisible(ui->wToolBarMain, false);
            toolBarSetVisible(ui->wToolbarContainers, false);
            disableToolBar();
            loadOnlineUsers();
        }else if(itemText == "Сертификаты"){
            loadCertList();
            toolBarSetVisible(ui->wToolBarAU, false);
            toolBarSetVisible(ui->wToolBarMain, true);
            toolBarSetVisible(ui->wToolbarContainers, false);
        }else{
            toolBarSetVisible(ui->wToolBarAU, false);
            toolBarSetVisible(ui->wToolBarMain, true);
            toolBarSetVisible(ui->wToolbarContainers, false);
            disableToolBar();
            ui->tableView->setModel(nullptr);
        }
    }
//    QString itemText = item->text(0);
//    if(itemText == "root"){
//        createRootList();
//        enableToolbar(false);
//    }else if(itemText == "Контейнеры"){
//        createContainersList();
//        enableToolbar(true);
//    }else if(itemText == "Сертификаты"){
//        createCertList();
//        enableToolbar(true);
//    }else if(itemText == "Пользователи"){
//        createUsersList();
//        enableToolbar(true);
//    }else
//        enableToolbar(true);

}

void MainWindow::connectToDatabase(Settings *sett, const QString &pwd)
{

    qDebug() << __FUNCTION__;

    QString host = sett->server();
    QString database = "arcirk";
    QString userName = sett->user();
    QString password = pwd;

    db->setSqlUser(userName);
    db->setSqlPwd(password);
    db->setHost(host);
    db->setDatabaseName(database);
    db->connect();

    if(_sett->launch_mode() == mixed)
        connectToWsServer();

    if (!isDbOpen()){
        QMessageBox::critical(this, "Ошибка", QString("Ошибка подключения к базе данных: %2").arg(db->lastError()));
    }

    sett->save();

}

void MainWindow::on_mnuConnect_triggered()
{
    qDebug() << __FUNCTION__;
    QMap<QString, QVariant> clientSett;
    clientSett.insert("ServerHost", m_client->options()[bConfFieldsWrapper::ServerHost].toString());
    clientSett.insert("ServerPort", m_client->options()[bConfFieldsWrapper::ServerPort].toInt());
    clientSett.insert("ServerUser", m_client->options()[bConfFieldsWrapper::User].toString());
    std::string dPwd = m_client->options()[bConfFieldsWrapper::Hash].toString().toStdString();
    QString pass;
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    clientSett.insert("Password", pass);

    auto dlg = new DialogConnection(_sett, clientSett, this);

    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){        
        if(db->isOpen())
            db->close();
        if(m_client->isStarted())
            m_client->close();

        if(_sett->launch_mode() == mixed){
            connectToDatabase(_sett, dlg->pwd());
        }else{
            QString pass;
            QString pwd = clientSett["Password"].toString();
            QString usr = clientSett["ServerUser"].toString();
            QString  hash = bWebSocket::generateHash(usr, pwd);
            m_client->options()[bConfFieldsWrapper::Hash] = hash;
            m_client->options()[bConfFieldsWrapper::User] = usr;
            m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
            m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
        }

       connectToWsServer();

       isDbOpen();
    }

}

void MainWindow::on_btnAdd_clicked()
{
    qDebug() << __FUNCTION__;

//    auto reg = Registry();
//    QString admin = reg.currentWindowsName();
//    qDebug() << admin;

//    auto item = ui->treeWidget->currentItem();
//    if(item){
//        if(item->text(0) == "Контейнеры"){

//            QStringList keys = reg.containersNodes();
//             foreach(const QString& key, keys){
//                qDebug() << key;
//            }
//        }
//    }else
//        return;

//    auto dlg = new DialogSelectDevice(this);
//    dlg->setModal(true);
//    dlg->exec();

//    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
//                                                 QDir::homePath(),
//                                                 QFileDialog::ShowDirsOnly
//                                                 | QFileDialog::DontResolveSymlinks);
//    if(dir != ""){
//        QFile file(dir + QDir::separator() + "name.key");
//        if(file.open(QIODevice::ReadOnly)){
//            QString data = QString::fromLocal8Bit(file.readAll());
//            qDebug() << data;
//            if(data.compare("0/\u0016-")){
//                data.replace("0/\u0016-", "");
//                auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));

//                if(result == QMessageBox::Yes){

//                }
//            }else
//               QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//        }else{
//            QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//        }
//    }

    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    if(currentNode == "Реестр"){
        setKeysToRegistry();
    }else if(currentNode == "Пользователи"){
        auto reg = Registry();
        QStringList users = reg.localUsers();
        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            QString userName = dlg->dialogResult();
        }
    }else if(currentNode == "Компьютеры"){
        auto dlg = new DialogComputer(this);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            auto result = dlg->computer();
            if(result["Ref"].toString().isEmpty()){
                QString ref =  QUuid::createUuid().toString();
                ref = ref.mid(1, ref.length() - 2);
                result["Ref"] = ref;
                bool r = db->insertSqlTableRow("Servers", result);
                //auto model = (QSqlQueryModel*)ui->tableView->model();
                //model->setQuery(model->query().lastQuery());
                //UpdateRowIcons();
                //ui->tableView->resizeColumnsToContents();
                 loadCimputers();
                if(!r)
                    qCritical() << __FUNCTION__ << "Ошибка добавления новой строки в таблицу 'Servers'!";
            }
        }
    }else if(currentNode == "Сертификаты"){

        QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                     QDir::homePath(),
                                                     "Файл сертификата (*.cer)");
        if(fileName != ""){
            QFile file(fileName);
            QFileInfo fileInfo(file.fileName());
            if(file.open(QIODevice::ReadOnly)){
                QByteArray data = file.readAll();
                QUuid _uuid = QUuid::createUuid();
                QString uuid = _uuid.toString().mid(1, 36);
                QSqlQuery sql = QSqlQuery(db->getDatabase());
                sql.prepare("INSERT INTO [dbo].[Certificates] ([FirstField] ,[SecondField] ,[Ref] ,[data]) "
                              "VALUES (?,?,?,?)");
                sql.addBindValue(fileInfo.baseName());
                sql.addBindValue(file.fileName());
                sql.addBindValue(uuid);
                sql.addBindValue(data);
                auto r = sql.exec();
                if(!r){
                    qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Certificates'!";
                    qCritical() << __FUNCTION__ << db->lastError();
                }
                else
                    loadCertList();
            }

//            if(file.open(QIODevice::ReadOnly)){
//                QString data = QString::fromLocal8Bit(file.readAll());
//                qDebug() << data;
//                if(data.compare("0/\u0016-")){
//                    data.replace("0/\u0016-", "");
//                    auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));

//                    if(result == QMessageBox::Yes){

//                    }
//                }else
//                   QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//            }else{
//                QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//            }
        }
    }

}

void MainWindow::on_btnToDatabase_clicked()
{
    if(!isDbOpen())
        return;

    auto table = ui->tableView;
    int row = table->currentIndex().row();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);
    QString currentKeyName = table->model()->index(row, 1).data().toString();

    if(currentNode == "Реестр"){
        if(treeItem->parent()->text(0).compare("Текущий пользователь")){
            if(!isContainerExists(currentKeyName)){
                auto result = QMessageBox::question(this, "Экспорт контейнера", "Экспортировать контейнер на сервер?");
                if(result == QMessageBox::Yes){
                    KeysContainer cnt = KeysContainer(currentUser->sid(), currentKeyName, db, this);
                    bool result = cnt.toDataBase();
                    if(!result)
                        QMessageBox::critical(this, "Ошибка", "Ошибка экспорта контейнера!");
                    else
                        QMessageBox::information(this, "Экспорт", "Контейнер успешно экспортирован на сервер!");
                }
            }else
                QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже есть на сервере!").arg(currentKeyName));
        }else{

        }
    }
    qDebug() << __FUNCTION__;
}

void MainWindow::on_btnDelete_clicked()
{
    if(!isDbOpen())
        return;

    auto table = ui->tableView;

    if(!table->currentIndex().isValid())
        return;

    int row = table->currentIndex().row();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }


    QString currentNode = treeItem->text(0);
    QString currentKeyName = table->model()->index(row, 0).data().toString();

    if(currentNode == "Реестр"){
        if(treeItem->parent()->text(0).compare("Текущий пользователь")){

                auto result = QMessageBox::question(this, "Удаление контейнера закрытого ключа", "Удалить контейнер?");
                if(result == QMessageBox::Yes){
                    bool result = Registry::deleteContainer(currentUser, currentKeyName);
                    if(!result)
                        QMessageBox::critical(this, "Ошибка", "Ошибка удаления контейнера!");
                    else{
                        QMessageBox::information(this, "Удаление контейнера закрытого ключа", "Контейнер успешно удален из реестра!");
                        table->model()->removeRow(row);
                        currentUser->setContainers(Registry::currentUserContainers(currentUser->sid()));
                    }
                }

        }else{

            if(currentNode == "Реестр"){

            }else if(currentNode == "Пользователи"){
        //        auto reg = Registry();
        //        QStringList users = reg.localUsers();
        //        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
        //        dlg->setModal(true);
        //        dlg->exec();
        //        if(dlg->result() == QDialog::Accepted){
        //            QString userName = dlg->dialogResult();
        //        }
            }
        }
    }else if(currentNode == "Компьютеры"){
        //
        auto result = QMessageBox::question(this, "Удаление строки", "Удалить выбранную строку?");
        if(result == QMessageBox::Yes){
            bool r = db->deleteSqlTableRow("Servers", ui->tableView->model()->index(row, 3).data().toString());
            if(!r)
                QMessageBox::critical(this, "Ошибка", "Ошибка удаления строки!");
            else
            {
//                auto model = (QSqlQueryModel*)ui->tableView->model();
//                model->setQuery(model->query().lastQuery());
//                UpdateRowIcons();
                loadCimputers();
                ui->tableView->resizeColumnsToContents();
            }
        }

    }
    qDebug() << __FUNCTION__;
}

void MainWindow::on_btnInstallToUser_clicked()
{
    if(!m_client->isStarted()){
        QMessageBox::critical(this, "Ошибка", "Сервер обмена не доступен!");
        return;
    }
    if(!db->isOpen()){
        QMessageBox::critical(this, "Ошибка", "Сервер баз данных не доступен!!");
        return;
    }
    auto model = new QSqlQueryModel(this);
    model->setQuery("SELECT [FirstField] AS Имя\n"
                    ",[Ref] AS Ссылка\n"
                    ",[host] AS Host\n"
                    ",[sid] AS SID\n"
                    ",[uuid] AS ID\n"
                    "FROM [dbo].[CertUsers]", db->getDatabase());

    auto dlg = new DialogSelectFromDataBase(model, "Выбор ползователя", this);
    dlg->setModal(true);
    dlg->exec();
    if(dlg->result() == QDialog::Accepted){

//        QJsonObject obj = QJsonObject();
//        obj.insert("uuid_agent", m_client->getUuidSession());
//        obj.insert("uuid_client", itr.key());
//        obj.insert("command", "clientShow");


//        QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
//        //qDebug() << param;

//        if(client->started()){
//            client->send_command("command_to_qt_client", "", param.toStdString());
//        }

    }
    qDebug() << __FUNCTION__;
}

void MainWindow::on_mnuOptions_triggered()
{
    auto optDlg = new DialogOptions(this);
    optDlg->setModal(true);
    optDlg->exec();

    if(optDlg->result() == QDialog::Accepted){
        //auto opt = ClientSettings("conf_qt_cert_manager.json");

    }

    qDebug() << __FUNCTION__;
}

void MainWindow::onConnectionSuccess()
{
    if(m_client->isStarted())
    {
        QString status;
        if(db->isOpen()){
            status = "SQL Server: " + _sett->server() + "  ";
        }
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
        infoBar->setText(status);

        QJsonDocument doc = QJsonDocument();
        QJsonObject obj = QJsonObject();
        obj.insert("table", true);
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("get_active_users", "", param);
    }

    qDebug() << __FUNCTION__;
}

void MainWindow::onCloseConnection()
{
    QString status;
    if(db->isOpen()){
        status = "SQL Server: " + _sett->server();
    }
    infoBar->setText(status);
    qDebug() << __FUNCTION__;
}

void MainWindow::onConnectedStatusChanged(bool status)
{
    qDebug() << __FUNCTION__;
    if(currentUser)
        currentUser->setOnline(status);
}

void MainWindow::onClientJoinEx(const QString& resp, const QString& ip_address, const QString& host_name, const QString& app_name)
{
    qDebug() << __FUNCTION__; // << qPrintable(resp);
//  qDebug() << __FUNCTION__ << ip_address << " " << host_name << " " << app_name;
    //MainWindow::onClientJoinEx {"name": "admin", "uuid": "d1ca0bc6-b6cb-4b90-941c-85fb9faf70a9", "uuid_user": "e7429c10-8070-40da-ae2e-dea1cb9ae371", "active": true}
    //MainWindow::onClientJoinEx "192.168.10.14"   "VMBUHSVR"   "qt_cert_manager"

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();
    QUuid uuid = QUuid::fromString(obj.value("uuid_user").toString());
    QString name = obj.value("name").toString();

    auto itr = m_actUsers.find(uuid);
    if(itr != m_actUsers.end())
        itr.value()->setOnline(true);
    else{
        if(!currentUser)
            return;
        CertUser * usr = nullptr;
        if(!currentUser->thisIsTheUser(name, host_name)){
            usr = new CertUser(this);
            usr->setName(name);
            usr->setDomain(host_name);
        }
        else {
            usr = currentUser;
            auto item = findTreeItem("currentUser");
            if(item){
                item->setText(0, QString("Текущий пользователь (%1)").arg(currentUser->name()) );
                if(item->childCount() == 0){
                    auto Root = addTreeNode("Доступные контейнеры", "currentUserAvailableContainers", ":/img/key.svg");
                    item->addChild(Root);
                    auto reg = addTreeNode("Реестр", "currentUserRegistry", ":/img/registry.ico");
                    Root->addChild(reg);
                    auto dev = addTreeNode("Устройства", "currentUserDivace", ":/img/Card_Reader.ico");
                    Root->addChild(dev);
                }
            }
        }
        usr->setOnline(true);
        usr->setUuid(uuid);
        m_actUsers.insert(uuid, usr);
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    if(currentNode == "Активные пользователи")
    {
        loadOnlineUsers();
    }

}

void MainWindow::onClientLeave(const QString &resp)
{
    qDebug() << __FUNCTION__;

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();
    QUuid uuid = QUuid::fromString(obj.value("uuid_user").toString());
    QString name = obj.value("name").toString();

    auto itr = m_actUsers.find(uuid);
    if(itr != m_actUsers.end()){
        itr.value()->setOnline(false);
    }
    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }
    QString currentNode = treeItem->text(0);

    if(currentNode == "Активные пользователи")
    {
        loadOnlineUsers();
    }
}

void MainWindow::onMessageReceived(const QString &msg, const QString &uuid, const QString &recipient, const QString &recipientName)
{
    qDebug() << __FUNCTION__;
}

void MainWindow::onDisplayError(const QString &what, const QString &err)
{
    qCritical() << __FUNCTION__ << what << ": " << err ;
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
//    auto item = ui->tableView-item(row, column);
//    if(item){
        QString itemText = index.data().toString();
        QTreeWidgetItem * child = nullptr;

        if(itemText == "Контейнеры"){
            child = ui->treeWidget->topLevelItem(0)->child(0);
        }else if(itemText == "Сертификаты"){
            child = ui->treeWidget->topLevelItem(0)->child(1);
        }else if(itemText == "Пользователи"){
            child = ui->treeWidget->topLevelItem(0)->child(2);
        }

        if(child){
            ui->treeWidget->setCurrentItem(child, 0);
            emit ui->treeWidget->itemClicked(child, 0);
        }

    //}
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    if(currentNode == "Реестр"){
        ui->btnAdd->setEnabled(false);
        ui->btnEdit->setEnabled(false);
        ui->btnDelete->setEnabled(true);
        ui->btnImportFromDatabase->setEnabled(true);
        ui->btnToDatabase->setEnabled(true);
    }else if(currentNode == "Компьютеры"){
        ui->btnAdd->setEnabled(true);
        ui->btnEdit->setEnabled(true);
        ui->btnDelete->setEnabled(true);
    }else{
        disableToolBar();
    }
}


void MainWindow::on_btnEdit_clicked()
{
    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == "Реестр"){

    }else if(currentNode == "Пользователи"){
//        auto reg = Registry();
//        QStringList users = reg.localUsers();
//        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
//        dlg->setModal(true);
//        dlg->exec();
//        if(dlg->result() == QDialog::Accepted){
//            QString userName = dlg->dialogResult();
//        }
    }else if(currentNode == "Компьютеры"){
        auto dlg = new DialogComputer(this);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", ui->tableView->model()->index(row, 2).data().toString());
        _row.insert("SecondField", ui->tableView->model()->index(row, 3).data().toString());
        _row.insert("Ref", ui->tableView->model()->index(row, 4).data().toString());
        _row.insert("ipadrr", ui->tableView->model()->index(row, 5).data().toString());
        _row.insert("isServer", ui->tableView->model()->index(row, 6).data().toInt());
        dlg->setComputer(_row);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            auto result = dlg->computer();
            if(!result["Ref"].toString().isEmpty()){
                bool r = db->updateSqlTableRow("Servers", result, result["Ref"].toString());
//                auto model = (QSqlQueryModel*)ui->tableView->model();
//                model->setQuery(model->query().lastQuery());
//                UpdateRowIcons();
                 loadCimputers();
               // ui->tableView->resizeColumnsToContents();
                if(!r)
                    qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";
            }
        }
    }
}

void MainWindow::onGetActiveUsers(const QString& resp){

//    ui->tableView->setModel(nullptr);
//    auto model = new QJsonTableModel(this);
//    model->setJsonText(resp);
//    ui->tableView->setModel(model);
//    ui->tableView->resizeColumnsToContents();

    if(!currentUser)
        return;

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();

    auto rows = obj.value("rows").toArray();

    for (auto row = rows.begin(); row != rows.end(); ++row){
        auto item = row->toObject();
        QString name = item.value("user_name").toString();
        QUuid user_uuid = QUuid::fromString(item.value("user_uuid").toString());
        QString host = item.value("host_name").toString();
        CertUser * user;
        if(!currentUser->thisIsTheUser(name, host)){
            user = new CertUser(this);
            user->setName(name);
            user->setDomain(host);
        }
        else{
             user = currentUser;
             if(db->isOpen()){
                 QString str = QString("select [Ref], [sid], [uuid] from [arcirk].[dbo].[CertUsers] where [FirstField] = '%1' AND [host] = '%2'").arg(name, host);
                 QSqlQuery query = QSqlQuery(str, db->getDatabase());
                 QString ref;
                 QString sid;
                 QString uuid;
                 QMap<QString, QVariant> _row;

                 while (query.next()) {
                     ref = query.value(0).toString();
                     sid = query.value(1).toString();
                     uuid = query.value(2).toString();
                     if(sid.isEmpty())
                         _row.insert("sid", currentUser->sid());
                     if(uuid.isEmpty())
                         _row.insert("uuid", user_uuid.toString());
                     break;
                 }
                 if(!ref.isEmpty() && _row.size() > 0){
                     currentUser->setRef(ref);
                     //update sid
                     db->updateSqlTableRow("CertUsers", _row, ref);
                 }
             }
        }
        user->setUuid(user_uuid);
        user->setOnline(true);
        m_actUsers.insert(user_uuid, user);
    }
}

void MainWindow::onParseCommand(const QString &result, int command)
{
    if(command == cmdCommand::echoUserName){
        currentUser->setName(result);
        currentUser->treeItem()->setText(0, QString("Текущий пользователь (%1)").arg(result));
        //terminal->send(QString("wmic useraccount where name='%1' get sid\n").arg(result), CommandLine::cmdCommand::wmicGetSID);
#ifdef _WINDOWS
        terminal->send(QString("WHOAMI /USER\n").arg(result), cmdCommand::wmicGetSID);
        m_client->setOsUserName(currentUser->name());
#endif
    }else if(command == cmdCommand::wmicGetSID){
        currentUser->setSid(result);
        terminal->send(QString("chcp\n").arg(result), cmdCommand::echoGetEncoding);
        if(!currentUser->sid().isEmpty()){
            QStringList curContainers = Registry::currentUserContainers(currentUser->sid());
            currentUser->setContainers(curContainers);
        }
    }else if(command == csptestGetConteiners){
        csptestGetContainers(result);
    }
}

void MainWindow::on_actionTest_triggered()
{
    //execute_command("test");

    if(terminal->listening()){
        //terminal->send("whoami /user & echo %username%\n");
        //wmic useraccount get name
        bool bOk;
        QString str = QInputDialog::getText( 0,
                                             "Ввод команды",
                                             "Команда:",
                                             QLineEdit::Normal,
                                             "",
                                             &bOk
        );
        if (bOk) {
            ui->txtTerminal->setText("");
            terminal->send(str + "\n", unknown);
            return;
        }
    }
}

#ifdef _WINDOWS
void MainWindow::execute_command(QString param)
{
     SHELLEXECUTEINFO ShExecInfo = {0};
     ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
     ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
     ShExecInfo.hwnd = NULL;
     ShExecInfo.lpVerb = "runas";
     ShExecInfo.lpFile = TEXT("powershell.exe");
     ShExecInfo.lpParameters = TEXT("stuff..");
     ShExecInfo.lpDirectory = NULL;
     ShExecInfo.nShow = SW_SHOW;
     ShExecInfo.hInstApp = NULL;
     ShellExecuteEx(&ShExecInfo);
     WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

void MainWindow::createConnectionsObjects()
{
    db = new SqlInterface(this);

    createWsObject(/*usr, pwd */);

}
#endif

void MainWindow::on_btnTermOptions_clicked()
{

    auto dlg = new DialogTerminalOptions(terminal, _sett, this);
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){

    }

}


void MainWindow::on_btnTerminalClear_clicked()
{
    ui->txtTerminal->setText("");
}


void MainWindow::on_btnCompToDatabase_clicked()
{

    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == "Активные пользователи"){

        QString hostName = ui->tableView->model()->index(row, 5).data().toString();
        if(isHostExists(hostName)){
            QMessageBox::critical(this, "Ошибка", "Компьютер уже зарегистрирован в базе!");
            return;
        }else{
            auto result =  QMessageBox::question(this, "Регистрация комьютера в базе", "Зарегистрировать компьютер пользователя в базе?");
            if(result != QMessageBox::Yes){
                return;
            }
        }
        QUuid _uuid = QUuid::createUuid();
        QString uuid = _uuid.toString().mid(1, 36);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", hostName);
        _row.insert("SecondField", hostName);
        _row.insert("Ref",uuid);
        _row.insert("ipadrr", ui->tableView->model()->index(row, 4).data().toString());
        _row.insert("isServer", 0);
        bool r = db->insertSqlTableRow("Servers", _row);
        if(!r)
            qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";

    }
}


void MainWindow::on_btnUserToDatabase_clicked()
{
    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == "Активные пользователи"){

        QString hostName = ui->tableView->model()->index(row, 5).data().toString();
        QString userName = ui->tableView->model()->index(row, 3).data().toString();

        if(isCertUserExists(hostName, userName)){
            QMessageBox::critical(this, "Ошибка", "Пользователь уже зарегистрирован в базе!");
            return;
        }else{
            auto result =  QMessageBox::question(this, "Регистрация пользователя в базе", "Зарегистрировать пользователя в базе для использования сертификатов?");
            if(result != QMessageBox::Yes){
                return;
            }
        }
        QUuid _uuid = QUuid::createUuid();
        QString uuid = _uuid.toString().mid(1, 36);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", userName);
        _row.insert("SecondField", userName);
        _row.insert("Ref", uuid);
        _row.insert("host", hostName);
        bool r = db->insertSqlTableRow("CertUsers", _row);
        if(!r){
            qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";
            QString err = db->lastError();
            if(!err.isEmpty())
                QMessageBox::critical(this, "Ошибка", "Ошибка регистрации в базе!");
        }
        else
            QMessageBox::information(this, "Успех", "Пользователь успешно зарегистрирован!");

    }
}


void MainWindow::on_btnContAdd_clicked()
{
    auto dlg = new DialogSelectDevice(this);
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){

        if(dlg->currentSelection() == 0){

            QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
            if(dir != ""){
                QFile file(dir + QDir::separator() + "name.key");
                if(file.open(QIODevice::ReadOnly)){
                    QString data = QString::fromLocal8Bit(file.readAll());
                    //qDebug() << data;
                    QRegularExpression ex("0/\u0016-");
                    if(ex.match(data).hasMatch()){
                        data.replace("0/\u0016-", "");
                        auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));
                        if(result == QMessageBox::Yes){

                        }
                    }else
                       QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }else{
                    QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }
            }
        }else if(dlg->currentSelection() == 1){

        }else if(dlg->currentSelection() == 2){

        }
    }
}

