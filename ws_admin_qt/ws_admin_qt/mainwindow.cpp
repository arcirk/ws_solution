
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdfx.h"
#include "serveresponse.h"

#ifdef _WINDOWS
#include <thread>
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

    treeChannelsObjects = ui->treeShannels;
    treeChannelsObjects->setColumnHidden(1, true);

    connect(this, SIGNAL(display_error(QString,QString)), this, SLOT(on_display_error(QString,QString)));
    connect(this, SIGNAL(display_notify(QString)), this, SLOT(on_display_notify(QString)));
    connect(this, SIGNAL(fill_tree_()), this, SLOT(fillTree()));
    connect(this, SIGNAL(fill_node(QString,QString)), this, SLOT(on_fill_node(QString,QString)));
    connect(this, SIGNAL(fill_group_tree(QString)), this, SLOT(on_fill_group_tree(QString)));

    popUp = new PopUp();

    ui->treeShannels->setVisible(false);

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
}

void MainWindow::update_branch(const QString &branch_name, const QString& serverResp){

    QTreeWidgetItem * item = findTreeItem(branch_name);
    if(item){
        //item->
    }else{

    }
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
        }

    }

    delete resp;

}

void MainWindow::on_action_4_triggered()
{
    QApplication::exit();
}

QTreeWidgetItem * MainWindow::findTreeItem(const QString &source, int col){

    QList<QTreeWidgetItem*> clist = treeServerObjects->findItems(source, Qt::MatchContains|Qt::MatchRecursive, col);
    if(clist.count() > 0)
        return clist[0];

    return nullptr;

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

    popUp->setPopupText(err);
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
                resizeColumns();
            }
        }
    }

}

void MainWindow::on_treeSrvObjects_itemSelectionChanged()
{
    auto item = treeServerObjects->currentItem();

    qDebug() << item->text(1);

    fillList(item->text(1));
}

void MainWindow::resizeColumns(){
    listChildServerObjects->horizontalHeader()->setResizeContentsPrecision(QHeaderView::Fixed);
//    for (int i = 0; i < listChildServerObjects->columnCount() - 1; ++i) {
//        listChildServerObjects->setColumnWidth(i, 200);
//    }
    listChildServerObjects->resizeColumnsToContents();
}

void MainWindow::fillList(const QString &nodeName) {
    listChildServerObjects->clear();
    listChildServerObjects->setColumnCount(0);
    listChildServerObjects->setRowCount(0);

    if (nodeName == "ServerName"){
        ServeResponse::loadTableFromJson(listChildServerObjects, settings->getJsonObject());
        resizeColumns();
        ui->treeShannels->setVisible(false);
    }else if (nodeName == "ActiveUsers"){
        ui->treeShannels->setVisible(false);
        client->send_command(std::string("get_active_users"), arc_json::nil_uuid(), std::string(""));
    }else if (nodeName == "Users"){
        ui->treeShannels->setVisible(true);
        client->get_group_list(client->get_app_uuid());
    }
}

void MainWindow::on_fill_group_tree(const QString &resp) {

    QString nil_uuid = QString::fromStdString(arc_json::nil_uuid());

    treeChannelsObjects->clear();
    treeChannelsObjects->setColumnCount(0);

//    auto * root = new QTreeWidgetItem(MainWindow::itTopItem);
//    root-> setText (0, "root");
//    root->setText(1, nil_uuid);
//    treeChannelsObjects->addTopLevelItem(root);

    QJsonDocument doc = ServeResponse::parseResp(resp);

    if (doc.isNull())
        return;

    auto * m_dataTable = new QTableWidget();
    if (doc.isArray()) {

        if (doc.array().count() == 0)
            return;

        QJsonObject reference = doc.array()[0].toObject();

        //Создаем таблицу значений для удобства поиска дочерних элементов
        ServeResponse::loadTableFromJson(m_dataTable, doc.array());
        //Создаем заголовки по первой структуре в массиве
        ServeResponse::set_header_tree(treeChannelsObjects, reference);
        //Индекс поля parent
        int iParent = ServeResponse::get_index_member(reference, "parent");
        int iColCount = (int)reference.count();

        //Заполняем дерево
        for (int i = 0; i < m_dataTable->rowCount() - 1; ++i) {
            QTableWidgetItem* parentValue = m_dataTable->item(i, iParent);
            if (parentValue->text().isEmpty() || parentValue->text() == nil_uuid){
//                QString name = m_dataTable->item(i, 0)->text();
//                QString id = m_dataTable->item(i, 0)->text();
                //QTreeWidgetItem * item = new_tree_item(name, )
            }
        }


//        QList<QTableWidgetItem *> m_ListParent = m_dataTable->findItems("parent", Qt::MatchEndsWith);
//        if (m_ListParent.empty() ){
//            return;
//        }
//        QTableWidgetItem * parent = m_ListParent[0];
//        int indexParentColumn = m_dataTable->column(parent);

//        for (int i = 0; i < m_dataTable->rowCount() - 1; ++i) {
//            QTableWidgetItem* parentValue = m_dataTable->item(i, indexParentColumn);
//            if (parentValue->text().isEmpty() || parentValue->text() == nil_uuid){
//                QString name = m_dataTable->item(i, 0)->text();
//                QString id = m_dataTable->item(i, 0)->text();
//                //QTreeWidgetItem * item = new_tree_item(name, )
//            }
//        }
    }

}

void MainWindow::fill_property_values(QJsonObject *jsonObject, QTreeWidgetItem *item) {

//    treeChannelsObjects->
//    for (auto row = jsonObject->constBegin(); row != jsonObject->constEnd(); ++row)
//    {
//
//    }
    
}

void MainWindow::load_group_tree(QTreeWidgetItem *currentItem, QTableWidget *initialData) {


}

QTreeWidgetItem * MainWindow::new_tree_item(const QString &text, const QString &toolType, const QString &nextColVal) {

    auto * item = new QTreeWidgetItem(MainWindow::itTopItem);
    item->setText(0, text);
    if (!toolType.isEmpty()){
        item->setToolTip(0, toolType);
    }
    if (!nextColVal.isEmpty()){
        item->setText(1, nextColVal);
    }
    return item;
}