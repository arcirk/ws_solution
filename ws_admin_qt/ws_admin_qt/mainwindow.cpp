
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdfx.h"
#include "serveresponse.h"

#include <QModelIndex>
#include <QHeaderView>

#include "userdialog.h"
#include "selectgroupdialog.h"
#include "groupdialog.h"

#ifdef _WINDOWS
#include <thread>
//#else
//#include <boost/thread/thread.hpp>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new appSettings();
    settings->init();

    _callback_message callback = std::bind(&MainWindow::ext_message, this, std::placeholders::_1);
    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback);

    QDockWidget *docObjectTree = ui->dockSrvObjects;
    docObjectTree->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    initItemList();
    initTreeObjectsList();

    treeChannelsObjects = ui->treeChannels;
    treeChannelsObjects->setColumnHidden(1, true);

    connect(this, SIGNAL(display_error(QString,QString)), this, SLOT(on_display_error(QString,QString)));
    connect(this, SIGNAL(display_notify(QString)), this, SLOT(on_display_notify(QString)));
    connect(this, SIGNAL(fill_tree_()), this, SLOT(fillTree()));
    connect(this, SIGNAL(fill_node(QString,QString)), this, SLOT(on_fill_node(QString,QString)));
    connect(this, SIGNAL(fill_group_tree(QString)), this, SLOT(on_fill_group_tree(QString)));
    connect(this, SIGNAL(fill_users(QString)), this, SLOT(on_fill_users(QString)));

    popUp = new PopUp();

    ui->toolBtnGroupBox->addStretch();
    ui->toolBottonsUsers->addStretch();
    group_panel_setVisible(false, false);

    view_mode_hierarchy = true;
}

void MainWindow::initItemList() {
    listChildServerObjects = ui->listChildSrvObjects;
}

void MainWindow::initTreeObjectsList() {

    treeServerObjects = ui->treeSrvObjects;
    QAction *pQCmd = ui->dockSrvObjects->toggleViewAction();
    ui->mnuView->addAction(pQCmd);
    treeServerObjects->setColumnCount(2);
    treeServerObjects->setColumnHidden(1, true);
    QStringList headers;
    headers << tr("Объекты");
    treeServerObjects->setHeaderLabels(headers);
    fillTree();
}

void MainWindow::ext_message(const std::string &msg){
    processServeResponse(msg);
}

MainWindow::~MainWindow()
{
    delete client;
    delete settings;
    delete popUp;
    delete ui;
}


void MainWindow::on_mnuOptions_triggered()
{
    optDlg = new OptionsDlg(settings);
    optDlg->setModal(true);
    optDlg->exec();

    settings->save_settings();

    delete optDlg;

    QTreeWidgetItem *rootItem = treeServerObjects->topLevelItem(0);
    QTreeWidgetItem *serverItem = rootItem->child(0);
    serverItem->setText(0, serverView());
    serverItem->setToolTip(0, serverView());
    serverItem->setText(1, "ServerName");

}

void MainWindow::fillTree(){

   treeServerObjects->clear();

   auto * root = new QTreeWidgetItem(MainWindow::itTopItem);
   root-> setText (0, "root");
   root->setText(1, "root");
   treeServerObjects->addTopLevelItem(root);

   auto * itemServer = new QTreeWidgetItem(MainWindow::itTopItem);
   itemServer->setText(0, serverView());
   itemServer->setToolTip(0, serverView());
   itemServer->setText(1, "ServerName");
   if(client->started()){
       auto * itemActiveUsers = new QTreeWidgetItem(MainWindow::itTopItem);
       itemActiveUsers->setText(0,"Активные подключения");
       itemActiveUsers->setToolTip(0, "Активные подключения");
       itemActiveUsers->setText(1, "ActiveUsers");
       itemServer->addChild(itemActiveUsers);
       auto * itemUsers = new QTreeWidgetItem(MainWindow::itTopItem);
       itemUsers->setText(0,"Пользователи");
       itemUsers->setToolTip(0, "Пользователи");
       itemUsers->setText(1, "Users");
       itemServer->addChild(itemUsers);
   }else
   {
       auto * itemErr = new QTreeWidgetItem(MainWindow::itTopItem);
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
    group_panel_setVisible(false, false);
}

QString MainWindow::serverView(){
    return settings->ServerName + " [" + settings->ServerHost + ":" + QString::number(settings->ServerPort) + "]";
}

void MainWindow::on_mnuStartSession_triggered()
{

    if (client->started()) {
        return;
    }

    if(settings->Hash.isEmpty()){
        return;
    }

    client->admin_name = settings->RootUser.toStdString();
    client->hash = settings->Hash.toStdString();
    client->host = settings->ServerHost.toStdString();
    client->port = settings->ServerPort;

    client->open();

//    #ifdef _WINDOWS
//    std::thread(std::bind(&MainWindow::start, this)).detach();
//    #else
//    boost::thread(boost::bind(&MainWindow::start, this)).detach();
//    #endif
}

void MainWindow::start(){
    client->open(false);
}

void MainWindow::processServeResponse(const std::string &response){

    std::string json;
    try {
        json = arc_json::base64_decode(response);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    if(json.empty())
        return;

    QString q_json = QString::fromStdString(json);
    auto resp = new ServeResponse(q_json);

    if(!resp->isParse){
        delete resp;
        return;
    }

    if(resp->result == "error"){

        emit display_error(resp->command, resp->message);

        if(resp->command == "set_client_param")
            client->close();
    }else
    {
        if(resp->command == "set_client_param"){
            //client->send_command(std::string("get_active_users"), arc_json::nil_uuid(), std::string(""));
            emit display_notify("Подключился к серверу.");
            emit fill_tree_();
        }else if(resp->command == "get_active_users"){
            emit fill_node(resp->command, resp->message);
        }else if(resp->command == "close_connections"){
            emit display_notify("Отключился от сервера.");
            emit fill_tree_();
        }else if (resp->command == "get_group_list"){
           fill_group_tree(resp->message);
        }else if (resp->command == "get_users"){
            //qDebug() << resp->message;
            fill_users(resp->message);
        }else if (resp->command == "add_user"){
            on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
        }else if (resp->command == "remove_user"){
            on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
        }else if (resp->command == "update_user"){
            on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
        }else if (resp->command == "add_group"){
            client->get_group_list(client->get_app_uuid());
        }else if (resp->command == "edit_group"){
            client->get_group_list(client->get_app_uuid());
        }else if (resp->command == "remove_group"){
            client->get_group_list(client->get_app_uuid());
        }else if (resp->command == "set_parent"){
            on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
        }else if (resp->command == "kill_session"){
            if (current_node == "ActiveUsers")
                fillList(current_node);
        }else if (resp->command == "client_leave"){
            if (current_node == "ActiveUsers")
                fillList(current_node);
        }else if (resp->command == "client_join") {
            if (current_node == "ActiveUsers")
                fillList(current_node);
        }else{
            emit display_notify("Команда без обработчика: " + resp->command);
        }
    }

    delete resp;

}

void MainWindow::on_action_4_triggered()
{
    QApplication::exit();
}

void MainWindow::on_mnuDisconnect_triggered()
{
    if(client->started()){
        client->close();
    }
}

void MainWindow::on_display_error(const QString& what, const QString& err) {
    //qDebug() << err;
    //QMessageBox::critical(nullptr, "Ошибка", err);

    popUp->setPopupText(what + ": " + err);
    popUp->show();
}

void MainWindow::on_display_notify(const QString &msg) {
    popUp->setPopupText(msg);
    popUp->show();
}

void MainWindow::on_fill_node(const QString& command, const QString& resp){

    if (command == "get_active_users") {

        QJsonDocument doc = ServeResponse::parseResp(resp);
        if (!doc.isNull()) {
            if (doc.isArray()) {
                ServeResponse::loadTableFromJson(listChildServerObjects, doc.array());
                set_header_aliases(listChildServerObjects);
                resizeColumns();
            }
        }
    }

}

void MainWindow::set_header_aliases(QTableWidget* table) {

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
    }
}

void MainWindow::on_treeSrvObjects_itemSelectionChanged()
{
    auto item = treeServerObjects->currentItem();

    if(item)
        fillList(item->text(1));
}

void MainWindow::resizeColumns(){
    listChildServerObjects->horizontalHeader()->setResizeContentsPrecision(QHeaderView::Fixed);
    for (int i = 0; i < listChildServerObjects->columnCount() ; ++i) {
        listChildServerObjects->resizeColumnToContents(i);
    }
}

void MainWindow::fillList(const QString &nodeName) {

    current_node = nodeName;

    listChildServerObjects->clear();
    listChildServerObjects->setColumnCount(0);
    listChildServerObjects->setRowCount(0);

    if (nodeName == "ServerName"){
        ServeResponse::loadTableFromJson(listChildServerObjects, settings->getJsonObject());
        resizeColumns();
        group_panel_setVisible(false, false);
    }else if (nodeName == "ActiveUsers"){
        group_panel_setVisible(false, true);
        client->send_command(std::string("get_active_users"), arc_json::nil_uuid(), std::string(""));
    }else if (nodeName == "Users"){
        group_panel_setVisible(true, false);
        client->get_group_list(client->get_app_uuid());
    }
}

void MainWindow::group_panel_setVisible(bool visible, bool isSessions) {
    ui->panelGroupUsers->setVisible(visible);
    ui->btnAddUser->setVisible(visible);
    ui->btnDeleteUser->setVisible(visible);
    ui->btnEditUser->setVisible(visible);
    ui->btnToGroup->setVisible(visible);
    ui->btnViewMode->setVisible(visible);
    ui->btnKillSession->setVisible(isSessions);
    ui->btnSendMessage->setVisible(isSessions);
}

void MainWindow::on_fill_group_tree(const QString &resp) {

    treeChannelsObjects->clear();
    treeChannelsObjects->setColumnCount(0);

    QJsonDocument doc = ServeResponse::parseResp(resp);
    if(doc.array().isEmpty())
        return;
    QJsonObject reference = doc.array()[0].toObject();
    QMap<QString, int> header = ServeResponse::get_header(&reference, "SecondField");

    QSortFilterProxyModel* model = ServeResponse::get_proxyModel(doc, header);

    if (!model)
        return;

    tree_group_create_columns(header, treeChannelsObjects);

    tree_create_root_items(model, treeChannelsObjects, header);

    group_header = header;

    auto headerItem = treeChannelsObjects->headerItem();
    headerItem->setText(group_header["SecondField"], "Группы пользователей");
}


void MainWindow::load_group_tree(QSortFilterProxyModel* model, QTreeWidgetItem* root, QMap<QString, int> header) {

    QString parentRef = root->text(header["Ref"]);

    if (parentRef.isEmpty())
        return;

    model->setFilterFixedString(parentRef);
    model->setFilterKeyColumn(header["Parent"]);

    for (int i = 0; i < model->rowCount(); ++i) {
        auto * item = new QTreeWidgetItem(MainWindow::itTopItem);
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
        load_group_tree(model, child, header);
    }
}

void MainWindow::tree_create_root_items(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header){

    auto * root = new QTreeWidgetItem(MainWindow::itTopItem);
    root-> setText (0, "root");
    root->setText(header["Ref"], "00000000-0000-0000-0000-000000000000");
    tree->addTopLevelItem(root);
    load_group_tree(model, root, header);
    tree->expandAll();

}

void MainWindow::tree_group_create_columns(QMap<QString, int> header, QTreeWidget* tree) {

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

void MainWindow::on_treeChannels_itemActivated(QTreeWidgetItem *item, int column)
{
    qDebug() << item->text(column) << "on_treeChannels_itemActivated";;
}


void MainWindow::on_treeChannels_itemChanged(QTreeWidgetItem *item, int column)
{
    qDebug() << item->text(column) << "on_treeChannels_itemChanged";
}


void MainWindow::on_treeChannels_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (view_mode_hierarchy){
        if (group_header.count() > 0 && current) {

            QString group_uuid = current->text(group_header["Ref"]);
            if (group_uuid.isEmpty())
                return;
            if (client->started()) {
                client->get_users(group_uuid.toStdString(), arc_json::nil_uuid());
            }

        }
    } else{
        if (group_header.count() > 0 && current) {

            QString group_uuid = current->text(group_header["Ref"]);
            if (group_uuid.isEmpty() || group_uuid == QString::fromStdString(arc_json::nil_uuid())){
                client->get_users("", arc_json::nil_uuid());
            }

        }
    }
}
void MainWindow::on_fill_users(const QString& resp){

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

    set_header_aliases(listChildServerObjects);

    for (int i = 0; i < listChildServerObjects->columnCount() ; ++i) {
        listChildServerObjects->resizeColumnToContents(i);
    }


//    QSortFilterProxyModel* model = ServeResponse::get_proxyModel(doc);
//
//    QJsonObject reference = doc.array()[0].toObject();
//    QMap<QString, int> header = ServeResponse::get_header(&reference);


}

void MainWindow::on_mnuServerRun_triggered()
{
    QString program = settings->ServerBinDir + "/ws_server";
    auto *myProcess = new QProcess(this);
    myProcess->setWorkingDirectory(settings->ServerBinDir);
    myProcess->start(program);
    myProcess->waitForFinished(-1);
}


void MainWindow::on_btnAddUser_clicked()
{
    if (!client->started())
    {
        popUp->setPopupText("Требуется подключение к серверу!");
        popUp->show();
        return;
    }

    auto * usr_info = new Ui::user_info();
    usr_info->new_user = true;
    QTreeWidgetItem * item = ui->treeChannels->currentItem();
    if (item){
        usr_info->parent = item->text(group_header["Ref"]);
    } else
        usr_info->parent = QString::fromStdString(arc_json::nil_uuid());

    auto * dlg = new UserDialog(this, usr_info);
    dlg->setModal(true);
    dlg->exec();

    if (usr_info->accepted){
        std::string param = usr_info->to_json();
        client->send_command("add_user", arc_json::nil_uuid(), param);
    }

    delete usr_info;

}


void MainWindow::on_listChildSrvObjects_itemActivated(QTableWidgetItem *item)
{
    if (current_node == "Users"){

        auto * usr_info = new Ui::user_info();

        try {
            usr_info->name = listChildServerObjects->item(listChildServerObjects->currentRow(), 0)->text();
            usr_info->pres = listChildServerObjects->item(listChildServerObjects->currentRow(), 1)->text();
            usr_info->role = listChildServerObjects->item(listChildServerObjects->currentRow(), 2)->text();
            usr_info->uuid = listChildServerObjects->item(listChildServerObjects->currentRow(), 3)->text();
            usr_info->parent = ui->treeChannels->currentItem()->text(group_header["Ref"]);
        }  catch (QException& e) {
            QMessageBox::critical(this, "Ошибка", e.what());
            return;
        }


        auto * dlg = new UserDialog(this, usr_info);
        dlg->setModal(true);
        dlg->exec();

        if (usr_info->accepted){

            std::string param = user_change_request_parameters(usr_info);
            client->send_command("update_user", arc_json::nil_uuid(), param);
        }

        delete usr_info;
    }

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
        client->remove_user(user_uuid.toStdString(), arc_json::nil_uuid());
    }

}

std::string MainWindow::user_change_request_parameters(Ui::user_info *usr_info) {

    QList<QString> fields;
    fields.push_back("FirstField");
    fields.push_back("SecondField");
    fields.push_back("role");

    if (!usr_info->password.isEmpty()){
        usr_info->hash = QString::fromStdString(arc_json::get_hash(usr_info->name.toStdString(), usr_info->password.toStdString()));
        fields.push_back("hash");
    }

    return usr_info->to_json_set_data(fields);

}
void MainWindow::on_btnEditUser_clicked()
{
    if (current_node == "Users"){

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
            usr_info->parent = ui->treeChannels->currentItem()->text(group_header["Ref"]);
        }  catch (QException& e) {
            QMessageBox::critical(this, "Ошибка", e.what());
            return;
        }

        auto * dlg = new UserDialog(this, usr_info);
        dlg->setModal(true);
        dlg->exec();

        if (usr_info->accepted){

            std::string param = user_change_request_parameters(usr_info);
            client->send_command("update_user", arc_json::nil_uuid(), param);
        }

        delete usr_info;
    }
}


void MainWindow::on_btnAddGroup_clicked()
{
    auto * info = new Ui::group_info();
    info->new_group = true;

    QTreeWidgetItem* item = ui->treeChannels->currentItem();
    if (item){
        info->parent_uuid = item->text(group_header["Ref"]);
        info->parent_name = item->text(group_header["SecondField"]);
    }
    else{
        info->parent_uuid = QString::fromStdString(arc_json::nil_uuid());
        info->parent_name = "root";
    }

    auto* dlg = new GroupDialog(this, info);
    dlg->setModal(true);
    dlg->exec();

    if (info ->accepted)
        client->add_group(info->group_name.toStdString(), info->group_presentation.toStdString(), info->parent_uuid.toStdString(), arc_json::nil_uuid());

    delete info;
}

void MainWindow::on_btnEditGroup_clicked()
{
    QTreeWidgetItem* item = ui->treeChannels->currentItem();

    if (!item){
        QMessageBox::critical(this, "Ошибка", "Не выбрана группа!");
        return;
    }

    if (item->text(group_header["Ref"]).isEmpty() || item->text(group_header["Ref"]) == QString::fromStdString(arc_json::nil_uuid())){
        QMessageBox::critical(this, "Ошибка", "Нельзя изменить текущую группу!");
        return;
    }

    auto * info = new Ui::group_info();
    info->group_uuid = item->text(group_header["Ref"]);
    info->group_name = item->text(group_header["FirstField"]);
    info->group_presentation = item->text(group_header["SecondField"]);
    QTreeWidgetItem* itemParent = item->parent();
    if (itemParent){
        info->parent_uuid = item->parent()->text(group_header["Ref"]);
        info->parent_name = item->parent()->text(group_header["SecondField"]);
    }
    else{
        info->parent_uuid = QString::fromStdString(arc_json::nil_uuid());
        info->parent_name = "root";
    }
    auto* dlg = new GroupDialog(this, info);
    dlg->setModal(true);
    dlg->exec();

    if (info ->accepted)
        client->edit_group(info->group_uuid.toStdString(), info->group_name.toStdString(), info->group_presentation.toStdString(), info->parent_uuid.toStdString(), arc_json::nil_uuid());

    delete info;
}


void MainWindow::on_btnDelGroup_clicked()
{
    if (current_node == "Users"){

        QTreeWidgetItem* current = ui->treeChannels->currentItem();

        if (!current){
            QMessageBox::critical(this, "Ошибка", "Не выбрана группа!");
            return;
        }

        if(current->text(group_header["Ref"]).isEmpty() || current->text(group_header["Ref"]) == QString::fromStdString(arc_json::nil_uuid())){
            QMessageBox::critical(this, "Ошибка", "Нельзя удалить текущую группу!");
            return;
        }

        auto result =  QMessageBox::question(this, "Удаление группы", "Удалить текущую группу?"
                                                                      "\nВсе подчиненные группы и пользователи этих групп будут удалены!"
                                                                      "\nВсе пользователи будут перемещены в корневую группу!");

        if(result == QMessageBox::Yes){
            QString user_uuid = current->text(group_header["Ref"]);
            client->remove_group(user_uuid.toStdString(), arc_json::nil_uuid());
        }

    }
}


void MainWindow::on_btnToGroup_clicked()
{
    if (current_node == "Users"){
        if (listChildServerObjects->currentRow() == -1){
            QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
            return;
        }
        auto* dlg = new SelectGroupDialog(this, treeChannelsObjects->model());
        dlg->setModal(true);
        dlg->exec();

        QTableWidgetItem* item = listChildServerObjects->item(listChildServerObjects->currentRow(), 3);

        if(dlg->dlgAccepted){
            client->set_parent(item->text().toStdString(), dlg->selected_group_uuid.toStdString(), arc_json::nil_uuid());
        }
    }
}


void MainWindow::on_action_triggered()
{
        QApplication::aboutQt();
}

void MainWindow::on_btnViewMode_clicked()
{
    view_mode_hierarchy = !view_mode_hierarchy;
    if(view_mode_hierarchy){
        on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
    }else{
        if (client->started()){
            client->get_users("", arc_json::nil_uuid());
        }
    }
}


void MainWindow::on_btnKillSession_triggered(QAction *arg1)
{
    //
}


void MainWindow::on_btnSendMessage_clicked()
{
    engine = new QQmlEngine(this);
    chat_qml = new QQmlComponent(engine, QUrl(QStringLiteral("qrc:/Chat.qml")), this);
    QMetaObject::invokeMethod(dialog, "show", Qt::ConnectionType::AutoConnection, QGenericReturnArgument(), Q_ARG(QVariant, QVariant(QString::fromStdString(arc_json::nil_uuid()))));
}


void MainWindow::on_btnKillSession_clicked()
{
    if (current_node == "ActiveUsers"){
        if (listChildServerObjects->currentRow() == -1){
            QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
            return;
        }

        QTableWidgetItem* item = listChildServerObjects->item(listChildServerObjects->currentRow(), 3);

        auto result =  QMessageBox::question(this, "Удаление сессии", "Удалить текущую сессию пользователя?");

        if(result == QMessageBox::Yes){
            client->kill_session(item->text().toStdString(), arc_json::nil_uuid());
        }

    }
}

