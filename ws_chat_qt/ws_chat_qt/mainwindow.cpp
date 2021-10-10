#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlApplicationEngine>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QQmlContext>
#include "settingsdialog.h"
#include <QTableWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qClient = new QmlWebSocket(this);

    ui->chatWidget->engine()->rootContext()->setContextProperty("wsClient", qClient);

    popUp = new PopUp();

    connect(qClient, SIGNAL(display_error(QString, QString)), this, SLOT(on_display_error(QString, QString)));
    connect(qClient, SIGNAL(display_notify(QString)), this, SLOT(on_display_notify(QString)));
    connect(qClient, SIGNAL(user_catalog(QString)), this, SLOT(load_user_catalog(QString)));

    treeUserCatalog = ui->treeServerObj;

    pageCount = 0;

    initTableActivePage();

}

MainWindow::~MainWindow()
{
    delete qClient;
    delete ui;
}

void MainWindow::on_display_error(const QString& what, const QString& err)
{
    popUp->setPopupText(what + ": " + err);
    popUp->show();
}

void MainWindow::on_display_notify(const QString &msg)
{
    popUp->setPopupText(msg);
    popUp->show();
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


void MainWindow::on_mnuQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::load_user_catalog(const QString &resp)
{
    treeUserCatalog->clear();
    treeUserCatalog->setColumnCount(0);

    QJsonDocument doc = ServeResponse::parseResp(resp);
    if(doc.array().isEmpty())
        return;

    QJsonObject reference = doc.array()[0].toObject();
    QMap<QString, int> header = ServeResponse::get_header(&reference, "SecondField");

    QSortFilterProxyModel* model = ServeResponse::get_proxyModel(doc, header);

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
        if (header["SecondField"] != i) {
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

void MainWindow::initTableActivePage()
{
    auto table = ui->tableActivePage;
    table->setColumnCount(2);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Свойство"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Значение"));

    int rowCount = 3;

   table->setRowCount(rowCount);

   for (int row = 0; row < rowCount ; ++row ) {
       auto itemKey = new QTableWidgetItem("root" + QString::number(row));
       itemKey->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
       table->setItem(row, 0, itemKey);
   }

}


void MainWindow::on_treeServerObj_itemActivated(QTreeWidgetItem *item, int column)
{
    pageCount++;
    QString pageName = "Page_" + QString::number(pageCount);
    emit qClient->nextChatPage(pageName, item->text(0));
}



void MainWindow::on_tableActivePage_itemClicked(QTableWidgetItem *item)
{

    emit qClient->setPage(item->row());
}

