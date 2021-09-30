#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "stdfx.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QTreeView>
#include <QListView>
#include "optionsdlg.h"
#include "AppSettings.h"
#include <QAbstractTableModel>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlDebuggingEnabler>

#include "../../ws_client/include/iws_client.h"

#include "popup.h"
#include "userdialog.h"
#include "chatboxdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ext_message(const std::string& msg);

    void start();

private slots:
    void on_mnuOptions_triggered();
    void on_mnuStartSession_triggered();
    void on_action_4_triggered();
    void on_mnuDisconnect_triggered();
    void on_display_error(const QString& what, const QString& err);
    void on_display_notify(const QString& msg);
    void fillTree();
    void on_fill_users(const QString& resp);
    void on_fill_node(const QString& command, const QString& resp);
    void on_treeSrvObjects_itemSelectionChanged();
    void on_fill_group_tree(const QString& resp);
    void on_treeChannels_itemActivated(QTreeWidgetItem *item, int column);
    void on_treeChannels_itemChanged(QTreeWidgetItem *item, int column);
    void on_treeChannels_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);


    void on_mnuServerRun_triggered();

    void on_btnAddUser_clicked();

    void on_listChildSrvObjects_itemActivated(QTableWidgetItem *item);

    void on_btnDeleteUser_clicked();

    void on_btnEditUser_clicked();

    void on_btnAddGroup_clicked();

    void on_btnEditGroup_clicked();

    void on_btnDelGroup_clicked();

    void on_btnToGroup_clicked();

    void on_action_triggered();

    void on_btnViewMode_clicked();

    void on_btnKillSession_triggered(QAction *arg1);

    void on_btnSendMessage_clicked();

    void on_btnKillSession_clicked();

private:
    enum treeItemType{itTopItem=1001,itGroupItem,itImageItem};

    Ui::MainWindow *ui;

    QTreeWidget* treeServerObjects;
    QTableWidget * listChildServerObjects;
    QTreeWidget * treeChannelsObjects;
    OptionsDlg * optDlg;
    appSettings * settings;
    QDockWidget *docObjectTree;
    PopUp *popUp;       // Объявляем объект всплывающего сообщения
    IClient * client;
    QMap<QString, int> group_header;
    QString current_node;
    bool view_mode_hierarchy;

//    QQmlEngine *engine;
//    QQmlComponent *chat_qml;
//    QObject *dialog = Q_NULLPTR;

    QString serverView();
    void processServeResponse(const std::string& response);

    void resizeColumns();
    void initItemList();
    void initTreeObjectsList();

    void fillList(const QString& nodeName);
    static void load_group_tree(QSortFilterProxyModel* model, QTreeWidgetItem* root, QMap<QString, int> header);
    static void tree_group_create_columns(QMap<QString, int> header, QTreeWidget* tree);
    static void tree_create_root_items(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header);
    static std::string user_change_request_parameters(Ui::user_info *usr_info);
    void group_panel_setVisible(bool visible, bool isSessions);
    void set_header_aliases(QTableWidget* table);

    void get_tree_model();

    signals:
    void display_error(const QString& what, const QString& err);
    void display_notify(const QString& msg);
    void fill_tree_();
    void fill_node(const QString& command, const QString& resp);
    void fill_group_tree(const QString& resp);
    void fill_users(const QString& resp);
};
#endif // MAINWINDOW_H
