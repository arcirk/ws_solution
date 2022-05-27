#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/optionsdialog.h"
#include "include/groupdialog.h"
#include "include/selectgroupdialog.h"
#include "include/aboutdialog.h"
#include <serveresponse.h>
#include <QAbstractItemModel>

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("conf_admin_console.json", false)
{
    ui->setupUi(this);

    lblStatusService = new QLabel(this);
    statusBar()->addWidget(lblStatusService);
    lblStatusSocket = new QLabel(this);
    statusBar()->addWidget(lblStatusSocket);

#ifndef _WINDOWS
    isServiceStarted = settings.statusService();
#else
    isServiceStarted = false;
#endif
    if(isServiceStarted)
        lblStatusService->setText("Сервис запущен на локальном компьютере.");
    else
        lblStatusService->setText("Сервис не запущен.");


    lblStatusSocket->setText("Статус подключения: Не активное");

    settings[bConfFieldsWrapper::AppName] = "admin_console";
    settings.save();

    client = new bWebSocket(this, settings.confFileName());

    connect(client, SIGNAL(connectedStatusChanged(bool)), this, SLOT(onConnectedStatusChanged(bool)));
    connect(client, SIGNAL(displayError(QString,QString)), this, SLOT(onDisplayError(QString,QString)));
    connect(client, SIGNAL(getActiveUsers(QString)), this, SLOT(onGetActiveUsers(QString)));
    connect(client, SIGNAL(getGroupList(QString)), this, SLOT(onGetGroupList(QString)));
    connect(client, SIGNAL(getListUsers(QString)), this, SLOT(onFillUsers(QString)));
    connect(client, SIGNAL(addGroupUsers(QString)), this, SLOT(onAddGroup(QString)));
    connect(client, SIGNAL(editGroupUsers(QString)), this, SLOT(onEditGroup(QString)));
    connect(client, SIGNAL(removeGroupUsers(QString)), this, SLOT(onRemoveGroup(QString)));
    connect(client, SIGNAL(addUser(QString)), this, SLOT(onAddUser(QString)));
    connect(client, SIGNAL(deleteUser(QString)), this, SLOT(onDeleteUser(QString)));
    connect(client, SIGNAL(updateUser(QString)), this, SLOT(onUpdateUser(QString)));
    connect(client, SIGNAL(setUserParent(QString)), this, SLOT(onSetUserParent(QString)));
    connect(client, SIGNAL(clientLeave(QString)), this, SLOT(onClientLeave(QString)));
    connect(client, SIGNAL(clientJoin(QString)), this, SLOT(onClientJoin(QString)));


    QAction *pQCmd = ui->dockSrvObjects->toggleViewAction();
    ui->mnuView->addAction(pQCmd);

    listChildServerObjects = ui->listChildSrvObjects;
    treeServerObjects = ui->treeSrvObjects;
    treeChannelsObjects = ui->treeChannels;
    treeChannelsObjects->setColumnHidden(1, true);

    ui->toolBtnGroupBox->addStretch();
    ui->toolBottonsUsers->addStretch();
    visibilityControl(false, false);

    view_mode_hierarchy = true;

    if(settings[bConfFieldsWrapper::AutoConnect].toBool()){
        client->open(settings[bConfFieldsWrapper::User].toString(), "");
    }

    QString httpPwd = settings[bConfFieldsWrapper::HSPassword].toString();
    if(!httpPwd.isEmpty())
        httpPwd = QString::fromStdString(ClientSettings::crypt(httpPwd.toStdString(), "my_key"));
    else
        httpPwd = "";
    httpService = new HttpService(this, settings[bConfFieldsWrapper::HSHost].toString(),
            settings[bConfFieldsWrapper::HSUser].toString(),
            httpPwd);

    connect(httpService, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReplyFinished(QNetworkReply*)));

}

QString MainWindow::serverView(){
    return settings[bConfFieldsWrapper::ServerName].toString() + " [" + settings[bConfFieldsWrapper::ServerHost].toString() + ":" + QString::number(settings[bConfFieldsWrapper::ServerPort].toInt()) + "]";
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
    visibilityControl(false, false);
}

void MainWindow::visibilityControl(bool visible, bool isSessions) {
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
    if(client->isStarted()){
        client->close();
    }
    delete ui;
}

void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_mnuOptions_triggered()
{
    auto optDlg = new OptionsDialog(this);
    optDlg->setModal(true);
    optDlg->exec();

    QString confFile = settings.confFileName();

    if(optDlg->result() == QDialog::Accepted){
        settings = ClientSettings(confFile, false);
        if(client->isStarted()){
            client->setWebDavSettingsToServer();
            client->setSqlOptions();
        }
        fillTree(client->isStarted());
        httpService->setHttpHost(settings[bConfFieldsWrapper::HSHost].toString());
        httpService->setHttpUser(settings[bConfFieldsWrapper::HSUser].toString());
        QString httpPwd = settings[bConfFieldsWrapper::HSPassword].toString();
        if(!httpPwd.isEmpty())
            httpPwd = QString::fromStdString(ClientSettings::crypt(httpPwd.toStdString(), "my_key"));
        else
            httpPwd = "";
        httpService->setHttpPassword(httpPwd);
    }

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

    if (settings[bConfFieldsWrapper::RunAsService].toBool()){

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
        if(settings[bConfFieldsWrapper::AutoConnect].toBool() && !client->isStarted())
            client->open(settings[bConfFieldsWrapper::User].toString(), "");
   }
}


void MainWindow::on_mnuStop_triggered()
{
    if(!isServiceStarted){
        qDebug() << "Сервис не запущен!";
        return;
    }

    if (settings[bConfFieldsWrapper::RunAsService].toBool()){

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

void MainWindow::onConnectedStatusChanged(bool status) {
    qDebug() << __FUNCTION__ << status;
    QString wsStatus = status ? "Активное" : "Не активное";
    lblStatusSocket->setText("Статус подключения: " + wsStatus);
    fillTree(status);
}

void MainWindow::onDisplayError(const QString &what, const QString &err)
{
    qDebug() << "error: " << what << ":" << err;
}


void MainWindow::on_mnuConnect_triggered()
{
    if (client->isStarted()) {
        return;
    }

    client->updateSettings();

    client->open(settings[bConfFieldsWrapper::User].toString(), "");
}


void MainWindow::on_mnuDisconnect_triggered()
{
    if (client->isStarted()) {
        client->close();
    }
}

void MainWindow::onGetActiveUsers(const QString &resp)
{
    QJsonDocument doc = ServeResponse::parseResp(resp);
    if (!doc.isNull()) {
        if (doc.isArray()) {
            ServeResponse::loadTableFromJson(listChildServerObjects, doc.array());
            setHeaderAliases(listChildServerObjects);
            resizeColumns();
        }
    }
}

void MainWindow::treeGroupCreateColumns(QMap<QString, int> header, QTreeWidget* tree) {

    tree->setColumnCount(header.count());
    auto item = new QTreeWidgetItem();

    foreach ( const auto& Key, header.keys() )
    {
        const auto& Value = header[Key];
        item->setText(Value, Key);
        //qDebug() << "\t{" << Key << "," << Value << "}";
    }
    tree->setHeaderItem(item);

    for (int i = 0; i < header.size() ; ++i) {
        if (header["SecondField"] != i) {
            tree->setColumnHidden(i, true);
        }
    }

    tree->resizeColumnToContents(header["SecondField"]);
}

void MainWindow::treeGroupCreateRootItems(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header){

    auto * root = new QTreeWidgetItem();
    root-> setText (0, "root");
    root->setText(header["Ref"], "00000000-0000-0000-0000-000000000000");
    tree->addTopLevelItem(root);
    fillTreeGroup(model, root, header);
    tree->expandAll();

}

void MainWindow::fillTreeGroup(QSortFilterProxyModel* model, QTreeWidgetItem* root, QMap<QString, int> header) {

    QString parentRef = root->text(header["Ref"]);

    if (parentRef.isEmpty())
        return;

    model->setFilterFixedString(parentRef);
    model->setFilterKeyColumn(header["Parent"]);

    for (int i = 0; i < model->rowCount(); ++i) {
        auto * item = new QTreeWidgetItem();
        foreach ( const auto& Key, header.keys() )
        {
            QModelIndex modelIndex = model->index(i, header[Key]);
            QVariant parentValue = model->data(modelIndex);
            QString value = parentValue.toString();

            item->setText(header[Key], value);
            item->setToolTip(header[Key], value);
        }
        root->addChild(item);
    }

    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem *child = root->child(i);
        fillTreeGroup(model, child, header);
    }
}

void MainWindow::onGetGroupList(const QString &resp)
{
    treeChannelsObjects->clear();
    treeChannelsObjects->setColumnCount(0);

    QJsonDocument doc = ServeResponse::parseResp(resp);
    QJsonArray rows = doc.array();
    if(rows.isEmpty())
        return;
    QJsonObject reference = doc.array()[0].toObject();
    QMap<QString, int> header = ServeResponse::get_header(&reference, "SecondField");

    QSortFilterProxyModel* model = ServeResponse::get_proxyModel(rows, header);

    if (!model)
        return;

    treeGroupCreateColumns(header, treeChannelsObjects);

    treeGroupCreateRootItems(model, treeChannelsObjects, header);

    treeGroupHeader = header;

    auto headerItem = treeChannelsObjects->headerItem();
    headerItem->setText(treeGroupHeader["SecondField"], "Группы пользователей");

}

void MainWindow::setHeaderAliases(QTableWidget* table) {

    for (int i = 0; i < table->columnCount(); ++i) {
        auto item = table->horizontalHeaderItem(i);
        if (item->text() == "app_name")
            item->setText("Приложение");
        else if (item->text() == "name")
            item->setText("Имя");
        else if (item->text() == "user_uuid")
            item->setText("Идентификатор пользователя");
        else if (item->text() == "uuid")
            item->setText("Идентификатор");
        else if (item->text() == "FirstField")
            item->setText("Имя");
        else if (item->text() == "SecondField")
            item->setText("Представление");
        else if (item->text() == "Ref")
            item->setText("Ссылка");
        else if (item->text() == "channel")
            item->setText("Группа");
        else if (item->text() == "role")
            item->setText("Роль");
        else if (item->text() == "ip_address")
            item->setText("IP адрес");
    }
}

void MainWindow::resizeColumns(){
    listChildServerObjects->horizontalHeader()->setResizeContentsPrecision(QHeaderView::Fixed);
    for (int i = 0; i < listChildServerObjects->columnCount() ; ++i) {
        listChildServerObjects->resizeColumnToContents(i);
    }
}

void MainWindow::on_treeSrvObjects_itemSelectionChanged()
{
    auto item = treeServerObjects->currentItem();

    if(item)
        fillList(item->text(1));
}

void MainWindow::fillList(const QString &nodeName) {

    currentNode = nodeName;

    listChildServerObjects->clear();
    listChildServerObjects->setColumnCount(0);
    listChildServerObjects->setRowCount(0);

    if (nodeName == "ServerName"){
        ServeResponse::loadTableFromJson(listChildServerObjects, settings.getJsonObject());
        //listChildServerObjects->resizeColumnsToContents();
        listChildServerObjects->resizeColumnToContents(0);
        listChildServerObjects->resizeColumnToContents(1);
        visibilityControl(false, false);      
    }else if (nodeName == "ActiveUsers"){
        visibilityControl(false, true);
        client->sendCommand("get_active_users");
    }else if (nodeName == "Users"){
        visibilityControl(true, false);
        client->sendCommand("get_group_list");
    }
}

void MainWindow::on_treeChannels_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (view_mode_hierarchy){
        if (treeGroupHeader.count() > 0 && current) {

            QString group_uuid = current->text(treeGroupHeader["Ref"]);
            if (group_uuid.isEmpty())
                return;
            if (client->isStarted()) {
                client->getUsers(group_uuid);
            }

        }
    } else{
        if (treeGroupHeader.count() > 0 && current) {

            QString group_uuid = current->text(treeGroupHeader["Ref"]);
            if (group_uuid.isEmpty() || group_uuid == QString::fromStdString(IClient::nil_string_uuid())){
                client->getUsers("");
            }

        }
    }
}

void MainWindow::onFillUsers(const QString& resp){

    listChildServerObjects->clear();
    listChildServerObjects->setColumnCount(0);
    listChildServerObjects->setRowCount(0);

    QJsonDocument doc = ServeResponse::parseResp(resp);
    if (doc.array().isEmpty())
        return;
    QJsonObject reference = doc.array()[0].toObject();
    QMap<QString, int> header;

    header.insert("FirstField", 0);
    header.insert("SecondField", 1);
    header.insert("role", 2);
    header.insert("Ref", 3);
    header.insert("channel", 4);

    ServeResponse::loadTableFromJson(listChildServerObjects, doc.array(), header);

    setHeaderAliases(listChildServerObjects);

    for (int i = 0; i < listChildServerObjects->columnCount() ; ++i) {
        listChildServerObjects->resizeColumnToContents(i);
    }

}

void MainWindow::onAddGroup(const QString &resp)
{
    client->sendCommand("get_group_list");
}

void MainWindow::onEditGroup(const QString &resp)
{
    client->sendCommand("get_group_list");
}

void MainWindow::onRemoveGroup(const QString &resp)
{
    client->sendCommand("get_group_list");
}

void MainWindow::onAddUser(const QString &resp)
{
    on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
}

void MainWindow::onDeleteUser(const QString &resp)
{
    on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
}

void MainWindow::onUpdateUser(const QString &resp)
{
    on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
}

void MainWindow::onSetUserParent(const QString &resp)
{
    on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
}

void MainWindow::onClientLeave(const QString &resp)
{
    if (currentNode == "ActiveUsers")
        fillList(currentNode);
}

void MainWindow::onClientJoin(const QString &resp)
{
    if (currentNode == "ActiveUsers")
        fillList(currentNode);
}

void MainWindow::onReplyFinished(QNetworkReply *reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
       if(status_code.isValid()){
           int status = status_code.toInt();
           if(status != 200)
                qDebug() << __FUNCTION__ << "Error: " << status << " " + reply->errorString() ;
           else
           {
               QByteArray data = reply->readAll();
               responseResultHsService(data);
           }
       }
}

void MainWindow::on_btnAddGroup_clicked()
{
    auto * info = new Ui::group_info();
    info->new_group = true;

    QTreeWidgetItem* item = ui->treeChannels->currentItem();
    if (item){
        info->parent_uuid = item->text(treeGroupHeader["Ref"]);
        info->parent_name = item->text(treeGroupHeader["SecondField"]);
    }
    else{
        info->parent_uuid = QString::fromStdString(IClient::nil_string_uuid());
        info->parent_name = "root";
    }

    auto* dlg = new GroupDialog(this, info);
    dlg->setModal(true);
    dlg->exec();

    if (info ->accepted)
        client->addGroup(info->group_name, info->group_presentation, info->parent_uuid);

    delete info;
}


void MainWindow::on_btnEditGroup_clicked()
{
    QTreeWidgetItem* item = ui->treeChannels->currentItem();

    if (!item){
        QMessageBox::critical(this, "Ошибка", "Не выбрана группа!");
        return;
    }

    if (item->text(treeGroupHeader["Ref"]).isEmpty() || item->text(treeGroupHeader["Ref"]) == bWebSocket::nilUuid()){
        QMessageBox::critical(this, "Ошибка", "Нельзя изменить текущую группу!");
        return;
    }

    auto * info = new Ui::group_info();
    info->group_uuid = item->text(treeGroupHeader["Ref"]);
    info->group_name = item->text(treeGroupHeader["FirstField"]);
    info->group_presentation = item->text(treeGroupHeader["SecondField"]);
    QTreeWidgetItem* itemParent = item->parent();
    if (itemParent){
        info->parent_uuid = item->parent()->text(treeGroupHeader["Ref"]);
        info->parent_name = item->parent()->text(treeGroupHeader["SecondField"]);
    }
    else{
        info->parent_uuid = bWebSocket::nilUuid();
        info->parent_name = "root";
    }
    auto* dlg = new GroupDialog(this, info);
    dlg->setModal(true);
    dlg->exec();

    if (info ->accepted)
        client->editGroup(info->group_uuid, info->group_name, info->group_presentation, info->parent_uuid);

    delete info;
}


void MainWindow::on_btnDelGroup_clicked()
{
    if (currentNode == "Users"){

        QTreeWidgetItem* current = ui->treeChannels->currentItem();

        if (!current){
            QMessageBox::critical(this, "Ошибка", "Не выбрана группа!");
            return;
        }

        if(current->text(treeGroupHeader["Ref"]).isEmpty() || current->text(treeGroupHeader["Ref"]) == QString::fromStdString(IClient::nil_string_uuid())){
            QMessageBox::critical(this, "Ошибка", "Нельзя удалить текущую группу!");
            return;
        }

        auto result =  QMessageBox::question(this, "Удаление группы", "Удалить текущую группу?"
                                                                      "\nВсе подчиненные группы удалены!"
                                                                      "\nВсе пользователи будут перемещены в корневую группу!");

        if(result == QMessageBox::Yes){
            QString uuid = current->text(treeGroupHeader["Ref"]);
            client->removeGroup(uuid);
        }

    }
}


void MainWindow::on_treeChannels_itemActivated(QTreeWidgetItem *item, int column)
{
    qDebug() << item->text(column) << "on_treeChannels_itemActivated";
}


void MainWindow::on_btnAddUser_clicked()
{
    if (!client->isStarted())
    {
//        popUp->setPopupText("Требуется подключение к серверу!");
//        popUp->show();
        return;
    }

    auto * usr_info = new Ui::user_info();
    usr_info->new_user = true;
    QTreeWidgetItem * item = ui->treeChannels->currentItem();
    if (item){
        usr_info->parent = item->text(treeGroupHeader["Ref"]);
    } else
        usr_info->parent = QString::fromStdString(IClient::nil_string_uuid());

    auto * dlg = new UserDialog(this, usr_info);
    dlg->setModal(true);
    dlg->exec();

    if (usr_info->accepted){
        std::string param = usr_info->to_json();
        client->sendCommand("add_user", "", QString::fromStdString(param));
    }

    delete usr_info;
}


void MainWindow::on_btnEditUser_clicked()
{
    if (currentNode == "Users"){

        if (listChildServerObjects->currentRow() == -1){
            QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
            return;
        }

        auto * usr_info = new Ui::user_info();

        try {
            usr_info->name = listChildServerObjects->item(listChildServerObjects->currentRow(), 0)->text();
            usr_info->pres = listChildServerObjects->item(listChildServerObjects->currentRow(), 1)->text();
            usr_info->role = listChildServerObjects->item(listChildServerObjects->currentRow(), 2)->text();
            usr_info->uuid = listChildServerObjects->item(listChildServerObjects->currentRow(), 3)->text();
            usr_info->parent = ui->treeChannels->currentItem()->text(treeGroupHeader["Ref"]);
        }  catch (QException& e) {
            QMessageBox::critical(this, "Ошибка", e.what());
            return;
        }

        auto * dlg = new UserDialog(this, usr_info);
        dlg->setModal(true);
        dlg->exec();

        if (usr_info->accepted){

            std::string param = user_change_request_parameters(usr_info);
            client->sendCommand("update_user", "", QString::fromStdString(param));
        }

        delete usr_info;
    }
}

std::string MainWindow::user_change_request_parameters(Ui::user_info *usr_info) {

    QList<QString> fields;
    fields.push_back("FirstField");
    fields.push_back("SecondField");
    fields.push_back("role");

    if (!usr_info->password.isEmpty()){
        usr_info->hash = QString::fromStdString(IClient::get_hash(usr_info->name.toStdString(), usr_info->password.toStdString()));
        fields.push_back("hash");
    }

    return usr_info->to_json_set_data(fields);

}

bool MainWindow::responseResultHsService(const QByteArray &result)
{
    auto doc = QJsonDocument::fromJson(result);
    if(!doc.isEmpty()){
        auto obj = doc.object();
        auto itr = obj.find("command");
        if(itr != obj.end()){
            QString cmd = itr.value().toString();
            if(cmd == "getUsers"){
                if(client) {
                    if(client->isStarted()){
                        client->sendCommand("sync_users", bWebSocket::nilUuid(), QString::fromStdString(result.toStdString()));
                    }
                }
            }

        }
    }else
        return false;
    return true;
}

void MainWindow::on_btnDeleteUser_clicked()
{
    QTableWidgetItem * item = listChildServerObjects->item(listChildServerObjects->currentRow(), 3);
    if (!item){
        QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
        return;
    }

    auto result =  QMessageBox::question(this, "Удаление пользователя", "Удалить текущего пользователя?");

    if(result == QMessageBox::Yes){
        QString user_uuid = item->text();
        client->removeUser(user_uuid);
    }
}


void MainWindow::on_listChildSrvObjects_itemActivated(QTableWidgetItem *item)
{
    if (currentNode == "Users"){

        auto * usr_info = new Ui::user_info();

        try {
            usr_info->name = listChildServerObjects->item(listChildServerObjects->currentRow(), 0)->text();
            usr_info->pres = listChildServerObjects->item(listChildServerObjects->currentRow(), 1)->text();
            usr_info->role = listChildServerObjects->item(listChildServerObjects->currentRow(), 2)->text();
            usr_info->uuid = listChildServerObjects->item(listChildServerObjects->currentRow(), 3)->text();
            usr_info->parent = ui->treeChannels->currentItem()->text(treeGroupHeader["Ref"]);
        }  catch (QException& e) {
            QMessageBox::critical(this, "Ошибка", e.what());
            return;
        }


        auto * dlg = new UserDialog(this, usr_info);
        dlg->setModal(true);
        dlg->exec();

        if (usr_info->accepted){

            std::string param = user_change_request_parameters(usr_info);
            client->sendCommand("update_user", "", QString::fromStdString(param));
        }

        delete usr_info;
    }
}


void MainWindow::on_btnViewMode_toggled(bool checked)
{
    view_mode_hierarchy = checked;
    if(view_mode_hierarchy){
        on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
    }else{
        if (client->isStarted()){
            client->getUsers("");
        }
    }
}


void MainWindow::on_btnToGroup_clicked()
{
    if (currentNode == "Users"){
        if (listChildServerObjects->currentRow() == -1){
            QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
            return;
        }
        auto* dlg = new SelectGroupDialog(this, treeChannelsObjects->model());
        dlg->setModal(true);
        dlg->exec();

        QTableWidgetItem* item = listChildServerObjects->item(listChildServerObjects->currentRow(), 3);

        if(dlg->dlgAccepted){
            client->setParent(item->text(), dlg->selected_group_uuid);
        }
    }
}


void MainWindow::on_btnKillSession_clicked()
{
    if (currentNode == "ActiveUsers"){
        if (listChildServerObjects->currentRow() == -1){
            QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
            return;
        }

        QTableWidgetItem* item = listChildServerObjects->item(listChildServerObjects->currentRow(), 3);

        auto result =  QMessageBox::question(this, "Удаление сессии", "Удалить текущую сессию пользователя?");

        if(result == QMessageBox::Yes){
            client->killSession(item->text());
        }

    }
}


void MainWindow::on_mnuAbout_triggered()
{
    auto optDlg = new AboutDialog(this);
    optDlg->setModal(true);
    optDlg->exec();
}


void MainWindow::on_mnuExportData_triggered()
{
    if(client->isStarted()){
        client->sendCommand("export_tables_to_ext");
    }
}


void MainWindow::on_mnuImportUsersFrom1C_triggered()
{
    httpService->request("getUsers");
}

