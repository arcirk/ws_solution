#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlApplicationEngine>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QQmlContext>
#include "settingsdialog.h"
#include <QTableWidget>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qClient = new QmlWebSocket(this);

    ui->chatWidget->engine()->rootContext()->setContextProperty("wsClient", qClient);

    popUp = new PopUp();

    connect(qClient, SIGNAL(display_error(QString, QString)), this, SLOT(do_display_error(QString, QString)));
    connect(qClient, SIGNAL(display_notify(QString)), this, SLOT(do_display_notify(QString)));
    connect(qClient, SIGNAL(user_catalog(QString)), this, SLOT(load_user_catalog(QString)));
    connect(qClient, SIGNAL(get_messages(QString)), this, SLOT(do_get_messages(QString)));
    connect(qClient, SIGNAL(closeConnection()), this, SLOT(do_close_connection()));

    treeUserCatalog = ui->treeServerObj;

    pageCount = 0;

    initActiveRoomsModel();
    ui->tableActivePage->setModel(m_ConnModel);

}

MainWindow::~MainWindow()
{
    delete qClient;
    delete ui;
}

void MainWindow::do_display_error(const QString& what, const QString& err)
{
    popUp->setPopupText(what + ": " + err);
    popUp->show();
}

void MainWindow::do_display_notify(const QString &msg)
{
    popUp->setPopupText(msg);
    popUp->show();
}

void MainWindow::do_close_connection()
{
    treeUserCatalog->clear();
}

void MainWindow::on_mnuQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::load_user_catalog(const QString &resp)
{

    QJsonDocument doc = ServeResponse::parseResp(resp);

//    //ToDo: удалить
//    QString saveFileName = "usersCatalog.json";
//    QFileInfo fileInfo(saveFileName);
//    QDir::setCurrent(fileInfo.path());
//
//    QFile jsonFile(saveFileName);
//    if (!jsonFile.open(QIODevice::WriteOnly))
//    {
//        return;
//    }
//
//    // Записываем текущий объект Json в файл
//    jsonFile.write(QJsonDocument(doc.object()).toJson(QJsonDocument::Indented));
//    jsonFile.close();   // Закрываем файл
//    ////

    treeUserCatalog->clear();
    treeUserCatalog->setColumnCount(0);

    QJsonObject table = doc.object();

    if (table.isEmpty())
        return;

    QJsonArray objHeader = table["columns"].toArray();
    QJsonArray objRows = table["rows"].toArray();

    if(objHeader.isEmpty())
        return;

    //QJsonObject reference = doc["rows"].array()[0].toObject();
    QMap<QString, int> header = ServeResponse::get_header(&objHeader); //ServeResponse::get_header(&reference, "SecondField");

    QSortFilterProxyModel* model = ServeResponse::get_proxyModel(objRows, header);

    if (!model)
        return;

    tree_group_create_columns(header, treeUserCatalog);

    tree_create_root_items(model, treeUserCatalog, header);

    group_header = header;

    auto headerItem = treeUserCatalog->headerItem();
    headerItem->setText(group_header["SecondField"], "Группы пользователей");
}

void MainWindow::tree_group_create_columns(QMap<QString, int> header, QTreeWidget* tree) {

    tree->setColumnCount(header.count());
    auto item = new QTreeWidgetItem();

    foreach ( const auto& Key, header.keys() )
    {
        const auto& Value = header[Key];
        item->setText(Value, Key);
    }
    tree->setHeaderItem(item);

    for (int i = 0; i < header.size() ; ++i) {
        if (header["SecondField"] != i ) {//&& header["Ref"] != i
            tree->setColumnHidden(i, true);
        }
    }

    tree->resizeColumnToContents(header["SecondField"]);
}

void MainWindow::tree_create_root_items(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header){

    auto * root = new QTreeWidgetItem(MainWindow::itTopItem);
    root-> setText (0, "root");
    root->setText(header["Ref"], "00000000-0000-0000-0000-000000000000");
    tree->addTopLevelItem(root);
    load_group_tree(model, root, header);
    tree->expandAll();

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

void MainWindow::initActiveRoomsModel()
{
    m_ConnModel = new QStandardItemModel();
    m_ConnModel->setHorizontalHeaderItem(0, new QStandardItem("User"));
    m_ConnModel->setHorizontalHeaderItem(1, new QStandardItem("Token"));
}


void MainWindow::on_treeServerObj_itemActivated(QTreeWidgetItem *item, int column)
{
//    pageCount++;
//    QString pageName = "Page_" + QString::number(pageCount);
//    emit qClient->nextChatPage(pageName, item->text(0));

    auto curr = treeUserCatalog->currentItem();

    if (qClient->getUserUUID() == curr->text(group_header["Ref"])){
        return;
    }

    qDebug() << curr->text(group_header["Ref"] );
    qDebug() << qClient->getUserUUID();

    QString uuid_recipient = item->text(group_header["Ref"] );

    long int current_date = QmlWebSocket::currentDate();
    long int start_date = QmlWebSocket::addDay(current_date, -10);

    qClient->getMessages(uuid_recipient, start_date, current_date);

    insert_room(curr->text(group_header["Ref"] ), curr->text(group_header["SecondField"] ));

}

void MainWindow::do_get_messages(const QString &resp)
{
    qDebug() << resp;

    auto _resp = ServeResponse::parseResp(resp);
//    //ToDo: test

    if (_resp.isArray()) {

        QStandardItemModel * tableModel = new QStandardItemModel;


//        QString saveFileName = "messages.json";
//        QFileInfo fileInfo(saveFileName);
//        QDir::setCurrent(fileInfo.path());
//
//        QFile jsonFile(saveFileName);
//        if (!jsonFile.open(QIODevice::WriteOnly))
//        {
//            return;
//        }
//
//        // Записываем текущий объект Json в файл
//        jsonFile.write(QJsonDocument(_resp.array()).toJson(QJsonDocument::Indented));
//        jsonFile.close();   // Закрываем файл
    }

}

void MainWindow::insert_room(const QString &uuid, const QString& name)
{
    auto proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(m_ConnModel);

    proxyModel->setFilterFixedString(uuid);
    proxyModel->setFilterKeyColumn(1);

    if (proxyModel->rowCount() == 0) {

        int rowCount = m_ConnModel->rowCount();
        m_ConnModel->setItem(rowCount, 1, new QStandardItem(uuid));
        m_ConnModel->setItem(rowCount, 0, new QStandardItem(name));
        QModelIndex newIndex = m_ConnModel->index(m_ConnModel->rowCount() - 1, 0);
        ui->tableActivePage->setCurrentIndex(newIndex);
        emit qClient->setPage(m_ConnModel->rowCount());
    }else{
        auto itemsList = m_ConnModel->findItems(name);
        if(!itemsList.isEmpty())
            emit qClient->setPage(itemsList[0]->row());
    }

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


void MainWindow::on_mnuDisconnect_triggered()
{
    if(qClient->isStarted()){
        qClient->close();
    }
}


void MainWindow::on_tableActivePage_clicked(const QModelIndex &index)
{

    emit qClient->setPage(index.row() + 1);
}

