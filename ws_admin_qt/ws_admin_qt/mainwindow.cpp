#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    treeServerObjects->setColumnCount(1);
    QStringList headers;
    headers << tr("Объекты");
    treeServerObjects->setHeaderLabels(headers);

    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort);

    fillTree();
}

MainWindow::~MainWindow()
{
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
}

void MainWindow::fillTree(){

   treeServerObjects->clear();

   QTreeWidgetItem * root = new QTreeWidgetItem(MainWindow::itTopItem);
   root-> setText (0, "root");
   treeServerObjects->addTopLevelItem(root);

   QTreeWidgetItem * itemServer = new QTreeWidgetItem(MainWindow::itTopItem);
   itemServer->setText(0, serverView());
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


}

