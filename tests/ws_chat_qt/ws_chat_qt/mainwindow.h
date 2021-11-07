#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmlwebsocket.h"
#include "popup.h"
#include "appsettings.h"
#include <QTreeWidget>
#include <QTableWidgetItem>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void do_display_error(const QString& what, const QString& err);
    void do_display_notify(const QString& msg);
    void do_close_connection();

private slots:
    void on_mnuQuit_triggered();
    void load_user_catalog(const QString& resp);
    void on_treeServerObj_itemActivated(QTreeWidgetItem *item, int column);
    void do_get_messages(const QString& resp);
    void insert_room(const QString &uuid, const QString& name);
    void on_mnuSettings_triggered();
    void on_mnuDisconnect_triggered();

    void on_tableActivePage_clicked(const QModelIndex &index);

private:
    enum treeItemType{itTopItem=1001,itGroupItem,itImageItem};
    Ui::MainWindow *ui;
    QmlWebSocket * qClient;
    PopUp *popUp;
    QTreeWidget *treeUserCatalog;
    QMap<QString, int> group_header;
    int pageCount;

    QStandardItemModel* m_ConnModel;

    void tree_group_create_columns(QMap<QString, int> header, QTreeWidget* tree);
    static void tree_create_root_items(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header);
    static void load_group_tree(QSortFilterProxyModel* model, QTreeWidgetItem* root, QMap<QString, int> header);

    void initActiveRoomsModel();
};
#endif // MAINWINDOW_H
