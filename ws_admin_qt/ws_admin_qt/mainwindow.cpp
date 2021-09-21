#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../ws_client/include/global.h"
#include <QJsonObject>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    treeServerObjects->expandAll();
}

void MainWindow::ext_message(const std::string &msg){
    processServeResponse(msg);
}

MainWindow::~MainWindow()
{
    if(client){
        if(client->started()){
            client->close();
        }
    }
    delete settings;
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
   root->addChild(itemServer);

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
    QJsonDocument doc(QJsonDocument::fromJson(q_json.toUtf8()));

    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            auto message = obj.find("message");
            auto result = obj.find("result");
            if(result.value().isString()){
                if(result.value().toString() == "error" && message->isString()){
                    QMessageBox::critical(nullptr, "Ошибка", message.value().toString());
//                    if(!client->started())
//                        fillTree();
                    return;
                }
            }else
            {
                auto command = obj.find("command");
                if(command->isString()){
                    if(command->toString() == "set_client_param"){
                        client->send_command(std::string("get_active_users"), arc_json::nil_uuid(), std::string(""));
                    }else if(command->toString() == "get_active_users"){
                        qDebug() << message->toString();
                    }
                }
            }
        }
        else
        {
            //qDebug() << "Document is not an object" << endl;
        }
    }


}

void MainWindow::on_action_4_triggered()
{
    QApplication::exit();
}

QTreeWidgetItem * MainWindow::findTreeItem(const QString &source){

    QList<QTreeWidgetItem*> clist = treeServerObjects->findItems(source, Qt::MatchContains|Qt::MatchRecursive, 0);
    if(clist.count() > 0)
        return clist[0];

    return nullptr;

}
