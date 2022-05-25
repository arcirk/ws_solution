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

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    toolBar.append(ui->btnAdd);
    toolBar.append(ui->btnEdit);
    toolBar.append(ui->btnDelete);

    _sett = new settings(this);

    currentUser = new CertUser(this);
    QStringList cmdLine = Registry::currentUser(currentUser);
    QStringList curContainers = Registry::currentUserContainers(currentUser->sid());
    currentUser->setContainers(curContainers);

    db = QSqlDatabase::addDatabase("QODBC");
    db.setConnectOptions();

    createTree();
    ui->horizontalLayout->addStretch();

    infoBar = new QLabel(this);
    ui->statusbar->addWidget(infoBar);
    infoBar->setText("Не подключен.");

    if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
        onReconnect(_sett, _sett->pwd());


    m_client = new bWebSocket(this, "conf_qt_cert_manager.json");
    m_client->options()[bConfFieldsWrapper::AppName] = "qt_cert_manager";
    if(m_client->options()[bConfFieldsWrapper::User].toString().isEmpty()){
        m_client->options()[bConfFieldsWrapper::User] = "admin";
        QString hash = bWebSocket::generateHash("admin", "admin");
        m_client->options()[bConfFieldsWrapper::Hash] = hash;
        m_client->options().save();
    }

    connectToWsServer();

    if(m_client->isStarted())
    {
        QString status;
        if(db.isOpen()){
            status = "SQL Server: " + _sett->server() + "  ";
        }
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
        infoBar->setText(status);
    }
}

MainWindow::~MainWindow()
{
    if(m_client)
        delete m_client;
    delete ui;
}

void MainWindow::createTree()
{
    auto tree = ui->treeWidget;   

    auto * root = new QTreeWidgetItem();
    root-> setText (0, "root");
    root->setIcon(0, QIcon(":/img/system_config_root.ico"));
    tree->addTopLevelItem(root);

    QString usrName = currentUser->name();

    auto * curr_user = new QTreeWidgetItem();
    curr_user->setText(0, QString("Текущий пользователь (%1)").arg(usrName));
    curr_user->setToolTip(0, QString("Текущий пользователь (%1)").arg(usrName));
    curr_user->setIcon(0, QIcon(":/img/userOptions.ico"));
    root->addChild(curr_user);
    auto * currUserReg = new QTreeWidgetItem();
    currUserReg->setText(0, "Реестр");
    currUserReg->setIcon(0, QIcon(":/img/registry.ico"));
    curr_user->addChild(currUserReg);

    auto * server = new QTreeWidgetItem();
    server->setText(0, "Сервер");
    server->setIcon(0, QIcon(":/img/sqlServer.png"));
    root->addChild(server);

    auto * containers = new QTreeWidgetItem();
    containers->setText(0, "Контейнеры");
    containers->setIcon(0, QIcon(":/img/key_password_lock_800.ico"));
    server->addChild(containers);

    auto * certs = new QTreeWidgetItem();
    certs->setText(0, "Сертификаты");
    certs->setIcon(0, QIcon(":/img/certificate.ico"));
    server->addChild(certs);

    auto * users = new QTreeWidgetItem();
    users->setText(0, "Пользователи");
    users->setIcon(0, QIcon(":/img/bootloader_users_person_people_6080.ico"));
    server->addChild(users);

    tree->expandAll();
}

void MainWindow::createRootList()
{
    auto table = ui->tableWidget;
    table->setColumnCount(0);
    table->setRowCount(0);

    table->setColumnCount(1);
    table->setRowCount(3);
    QStringList cols = {"root"};
    table->setHorizontalHeaderLabels(cols);

    auto itemKeys = new QTableWidgetItem("Контейнеры");
    itemKeys->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemKeys->setIcon(QIcon(":/img/key_password_lock_800.ico"));
    table->setItem(0, 0, itemKeys);
    auto itemCerts = new QTableWidgetItem("Сертификаты");
    itemCerts->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemCerts->setIcon(QIcon(":/img/certificate.ico"));
    table->setItem(1, 0, itemCerts);
    auto itemUsers = new QTableWidgetItem("Пользователи");
    itemUsers->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    itemUsers->setIcon(QIcon(":/img/bootloader_users_person_people_6080.ico"));
    table->setItem(2, 0, itemUsers);

    table->resizeColumnsToContents();

}

void MainWindow::loadContainersList()
{
    auto table = ui->tableWidget;
    table->setColumnCount(0);
    table->setRowCount(0);
    table->setColumnCount(2);
    QStringList cols = {"Наименование", "id"};
    table->setHorizontalHeaderLabels(cols);
    table->setColumnHidden(1, true);

    if(db.isOpen()){
        QSqlQuery query("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Containers]");
        int i = 0;
        while (query.next()) {
            table->setRowCount(table->rowCount()+ 1);
            int id = query.value(0).toInt();
            QString name = query.value(1).toString().trimmed();
            auto itemTable = new QTableWidgetItem(name);
            itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
            table->setItem(i, 0, itemTable);
            auto itemId = new QTableWidgetItem(QString::number(id));
            itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(i, 1, itemId);
            i++;
        }
    }

    table->resizeColumnsToContents();
}

void MainWindow::LoadUsersList()
{
    auto table = ui->tableWidget;
    table->setColumnCount(0);
    table->setRowCount(0);

    ui->btnAdd->setEnabled(true);
}

void MainWindow::createCertList()
{
    auto table = ui->tableWidget;
    table->setColumnCount(0);
    table->setRowCount(0);
}

void MainWindow::createUsersList()
{
    auto table = ui->tableWidget;
    table->setColumnCount(0);
    table->setRowCount(0);
}

void MainWindow::enableToolbar(bool value)
{
    for (auto btn : toolBar) {
        btn->setEnabled(value);
    }
}

void MainWindow::loadItemChilds(QTreeWidgetItem *item)
{
    auto table = ui->tableWidget;
    table->setRowCount(0);
    table->setColumnCount(0);
    table->setRowCount(0);
    table->setColumnCount(1);
    table->setRowCount(item->childCount());
    QStringList cols = {"root"};
    table->setHorizontalHeaderLabels(cols);
    for (int i = 0; i < item->childCount(); i++) {
        auto child = item->child(i);
        auto itemTable = new QTableWidgetItem(child->text(0));
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
        table->setItem(i, 0, itemTable);
    }
    table->resizeColumnsToContents();
}

void MainWindow::loadItemSpecific(QTreeWidgetItem *item)
{

}

void MainWindow::loadKeysOnRegistry(CertUser *usr)
{
    QStringList keys = usr->containers();
    auto table = ui->tableWidget;
    table->setRowCount(0);
    table->setColumnCount(0);
    table->setRowCount(0);
    table->setColumnCount(2);
    table->setRowCount(keys.size());
    QStringList cols = {"","Наименование"};
    table->setHorizontalHeaderLabels(cols);
    auto colDb = table->horizontalHeaderItem(0);
    colDb->setIcon(QIcon(":/img/externalDataTable.png"));
    int i = 0;
    foreach(const QString& key, keys){
        auto itemTable = new QTableWidgetItem(key);
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //itemTable->setIcon(QIcon(":/img/key_password_lock_800.ico"));
        table->setItem(i, 1, itemTable);
        if(isContainerExists(key)){
//            auto itemIco = new QTableWidgetItem();
//            itemIco->setIcon(QIcon(":/img/checked.png"));
//            table->setItem(i, 0, itemIco);

            QLabel *lbl_item = new QLabel();
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/checked.png")));// *ui->my_label->pixmap());
            lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            table->setCellWidget(i, 0, lbl_item);
        }
        i++;
    }
    table->resizeColumnsToContents();
}

void MainWindow::disableToolBar()
{
    ui->btnAdd->setEnabled(false);
    ui->btnEdit->setEnabled(false);
    ui->btnDelete->setEnabled(false);
    ui->btnImportFromDatabase->setEnabled(false);
    ui->btnToDatabase->setEnabled(false);
}

bool MainWindow::isContainerExists(const QString &name)
{
    if(db.isOpen()){
        QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Containers] WHERE [FirstField] = '%1'").arg(name));
        while (query.next()) {
            return true;
        }
    }

    return false;
}
bool MainWindow::isUserExists(const QString &name)
{
    if(db.isOpen()){
        QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[users] WHERE [FirstField] = '%1'").arg(name));
        while (query.next()) {
            return true;
        }
    }

    return false;
}
void MainWindow::userToDatabase(const QString &name)
{
    if(isUserExists(name))
    {
        QMessageBox::information(this, "Пользователь", "Пользователь уже есть на севрвере!");
    }
}

void MainWindow::connectToWsServer()
{
    if(!db.isOpen())
        return;
    if(!m_client)
        return;
    if(m_client->isStarted())
        return;

    auto result = db.exec("select [host], [port] from dbo.WSConf;");
    if(result.lastError().type() == QSqlError::NoError){
        while (result.next()){
            QString _host = result.value(0).toString();
            int _port = result.value(1).toInt();
            m_client->setHost(_host);
            m_client->setPort(_port);
            m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
        }

    }

}

//void MainWindow::getUsersInDatabase()
//{
//    m_users.clear();

//    QSqlQuery query("SELECT [id]"
//                    ",[name]"
//                    ",[uuid]"
//                    ",[sid]"
//                    ",[domain]"
//                    "FROM [CertManager].[dbo].[users]");
//    while (query.next()) {
//        //auto usr = new User();
//        //usr->ge
//    }

//}


void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}


void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

    if(item->childCount() > 0){
        loadItemChilds(item);
        disableToolBar();
    }else{
        QString itemText = item->text(0);
        if(itemText == "Реестр"){
            ui->btnAdd->setEnabled(true);

            if(item->parent()->text(0).compare("Текущий пользователь")){
                loadKeysOnRegistry(currentUser);
            }else{

            }
        }else if(itemText == "Контейнеры"){
            loadContainersList();
        }else if(itemText == "Пользователи"){
            LoadUsersList();
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


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    auto item = ui->tableWidget->item(row, column);
    if(item){
        QString itemText = item->text();
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

    }
}

void MainWindow::onReconnect(settings *sett, const QString &pwd)
{

    QString host = sett->server();
    QString database = "arcirk";
    QString userName = sett->user();
    QString password = pwd;

//    QString connectString = "Driver={SQL Server Native Client 10.0};"; // Driver is now {SQL Server}
//    connectString.append("Server=%1;"); // IP,Port
//    connectString.append("Database=%2;");  // Schema
//    connectString.append("Uid=%3;");           // User
//    connectString.append("Pwd=%4;");           // Pass
//    db.setDatabaseName(QString(connectString).arg(host, database, userName, password));

    db.setDatabaseName(QString("DRIVER={SQL Server};"
                "SERVER=%1;DATABASE=%2;Persist Security Info=true;"
                "uid=%3;pwd=%4")
              .arg(host, database, userName, password));

//    QString connectString = "Driver={SQL Server Native Client 10.0};"; // Driver can also be {SQL Server Native Client 11.0}
//    connectString.append("Server=SERVERHOSTNAME\\SQLINSTANCENAME;");   // Hostname,SQL-Server Instance
//    connectString.append("Database=SQLDBSCHEMA;");  // Schema
//    connectString.append("Uid=SQLUSER;");           // User
//    connectString.append("Pwd=SQLPASS;");           // Pass
//    db.setDatabaseName(connectString);


    if (!db.open()){
        QMessageBox::critical(this, "Ошибка", QString("Ошибка подключения к базе данных: %2").arg(db.lastError().text()));
        infoBar->setText("Не подключен.");
    }else
        infoBar->setText("Поключен к " + sett->server());

    sett->save();
}


void MainWindow::on_mnuConnect_triggered()
{
    auto dlg = new DialogConnection(_sett, this);

    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){
       onReconnect(_sett, dlg->pwd());
    }

}


void MainWindow::on_btnAdd_clicked()
{

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

    if(!db.isOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    if(currentNode == "Реестр"){

    }else if(currentNode == "Пользователи"){
        auto reg = Registry();
        QStringList users = reg.localUsers();
        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            QString userName = dlg->dialogResult();
        }
    }

}


void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
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
    }else{
        disableToolBar();
    }
}


void MainWindow::on_btnToDatabase_clicked()
{
    if(!db.isOpen())
        return;

    auto table = ui->tableWidget;
    int row = table->currentRow();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);
    QString currentKeyName = table->item(row, 0)->text();

    if(currentNode == "Реестр"){
        if(treeItem->parent()->text(0).compare("Текущий пользователь")){
            if(!isContainerExists(currentKeyName)){
                auto result = QMessageBox::question(this, "Экспорт закрытого ключа", "Экспортировать закрытый ключ?");
                if(result == QMessageBox::Yes){
                    KeysContainer cnt = KeysContainer(currentUser->sid(), currentKeyName, this);
                    bool result = cnt.toDataBase();
                    if(!result)
                        QMessageBox::critical(this, "Ошибка", "Ошибка экспорта контейнера!");
                    else
                        QMessageBox::information(this, "Экспорт", "Ключ успешно экспортирован на сервер!");
                }
            }else
                QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже есть на сервере!").arg(currentKeyName));
        }else{

        }
    }
}


void MainWindow::on_btnDelete_clicked()
{
    if(!db.isOpen())
        return;

    auto table = ui->tableWidget;
    int row = table->currentRow();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }


    QString currentNode = treeItem->text(0);
    QString currentKeyName = table->item(row, 0)->text();

    if(currentNode == "Реестр"){
        if(treeItem->parent()->text(0).compare("Текущий пользователь")){

                auto result = QMessageBox::question(this, "Удаление контейнера закрытого ключа", "Удалить контейнер?");
                if(result == QMessageBox::Yes){
                    bool result = Registry::deleteContainer(currentUser, currentKeyName);
                    if(!result)
                        QMessageBox::critical(this, "Ошибка", "Ошибка удаления контейнера!");
                    else{
                        QMessageBox::information(this, "Удаление контейнера закрытого ключа", "Контейнер успешно удален из реестра!");
                        table->removeRow(row);
                        currentUser->setContainers(Registry::currentUserContainers(currentUser->sid()));
                    }
                }

        }else{

        }
    }
}


void MainWindow::on_btnInstallToUser_clicked()
{

}


void MainWindow::on_mnuOptions_triggered()
{
    auto optDlg = new DialogOptions(this);
    optDlg->setModal(true);
    optDlg->exec();

    if(optDlg->result() == QDialog::Accepted){
        //auto opt = ClientSettings("conf_qt_cert_manager.json");

    }

}

