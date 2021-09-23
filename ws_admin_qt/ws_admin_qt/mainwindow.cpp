
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdfx.h"
//#include "iws_client.h"

//#include "../../ws_client/include/global.h"
#include "serveresponse.h"
//#include <QJsonObject>
//#include <QJsonDocument>

#ifdef _WINDOWS
#include <thread>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qRegisterMetaType("QtMsgType");
    //connect(this, &ui::logFromMainThread, this, &ui::logFromMainThreadSlot);

    QDockWidget *docObjectTree = new QDockWidget(tr("Дерево объектов сервера"), this);
    docObjectTree->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    treeServerObjects = new QTreeWidget(docObjectTree);

    docObjectTree->setWidget(treeServerObjects);
    addDockWidget(Qt::LeftDockWidgetArea, docObjectTree);

    //listServerObjects = new QListWidget(this);

    QAction *pQCmd = docObjectTree->toggleViewAction();
    ui->mnuView->addAction(pQCmd);

    settings = new appSettings();
    settings->init();

    QRect rc = docObjectTree->geometry();
    rc.setLeft(10);

    docObjectTree->setGeometry(rc);

    treeServerObjects->setColumnCount(2);
    treeServerObjects->setColumnHidden(1, true);

    QStringList headers;
    headers << tr("Объекты");
    treeServerObjects->setHeaderLabels(headers);

    _callback_message callback = std::bind(&MainWindow::ext_message, this, std::placeholders::_1);

    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback);

    fillTree();

    connect(this, SIGNAL(display_error(const QString&,const QString&)),
                     this,
                     SLOT(on_display_error(const QString&,const QString&)));

    connect(this, SIGNAL(display_notify(const QString&)),
            this,
            SLOT(on_display_notify(const QString&)));

    connect(this, SIGNAL(fill_tree_()),
            this,
            SLOT(fillTree()));

    connect(this, SIGNAL(fill_node(const QString&,const QString&)),
            this,
            SLOT(on_fill_node(const QString&,const QString&)));

    popUp = new PopUp();

    treeChObjects = ui->treeChildObjectServers;
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

   QTreeWidgetItem * root = new QTreeWidgetItem(MainWindow::itTopItem);
   root-> setText (0, "root");
   treeServerObjects->addTopLevelItem(root);

   QTreeWidgetItem * itemServer = new QTreeWidgetItem(MainWindow::itTopItem);
   itemServer->setText(0, serverView());
   itemServer->setToolTip(0, serverView());
   itemServer->setText(1, "ServerName");
   if(client->started()){
       QTreeWidgetItem * itemActiveUsers = new QTreeWidgetItem(MainWindow::itTopItem);
       itemActiveUsers->setText(0,"Активные подключения");
       itemActiveUsers->setToolTip(0, "Активные подключения");
       itemActiveUsers->setText(1, "ActiveUsers");
       itemServer->addChild(itemActiveUsers);
   }else
   {
       QTreeWidgetItem * itemErr = new QTreeWidgetItem(MainWindow::itTopItem);
       itemErr->setText(0,"Сервер не доступен");
       itemErr->setToolTip(0, "Сервер не доступен");
       itemServer->addChild(itemErr);
   }
   root->addChild(itemServer);

   treeServerObjects->expandAll();
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

        display_error(resp->command, resp->message);

        if(resp->command == "set_client_param")
            client->close();
    }else
    {
        if(resp->command == "set_client_param"){
            client->send_command(std::string("get_active_users"), arc_json::nil_uuid(), std::string(""));
            display_notify("Подключился к серверу.");
            fill_tree_();
        }else if(resp->command == "get_active_users"){
            //qDebug() << resp->message;
            //display_notify()
            fill_node(resp->command, resp->message);
        }else if(resp->command == "close_connections"){
            display_notify("Отключился от сервера.");
            fill_tree_();
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

    if (command == "get_active_users"){
       // treeChObjects->setModel(nullptr);

        QJsonDocument doc = ServeResponse::parseResp(resp);
        if (!doc.isNull()){
            if (doc.isArray()){
                int rowCount = doc.array().count();
                auto * model = new QStandardItemModel(rowCount, 4);
                int index = 0;
                for (auto row : doc.array()){
                    if (row.isObject()){
                        QString name = ServeResponse::getStringMember(row, "name");
                        auto *item = new QStandardItem(name);
                        model->setItem(index++, 0, item);
                    }
                }
                treeChObjects->setModel(model);
            }
        }

//        auto * model = new QStandardItemModel(4, 4);
//        for (int row = 0; row < 4; ++row) {
//            for (int column = 0; column < 4; ++column) {
//                auto *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
//                model->setItem(row, column, item);
//            }
//        }
//        treeChObjects->setModel(model);
    }


}