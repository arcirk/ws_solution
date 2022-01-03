#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <clientsettings.h>
#include <qmlwebsocket.h>
#include "include/userdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_mnuExit_triggered();
    void on_mnuOptions_triggered();
    void on_mnuInstallService_triggered();
    void on_mnuStart_triggered();
    void on_mnuStop_triggered();
    void on_mnuStatusService_triggered();
    void on_mnuConnect_triggered();
    void on_mnuDisconnect_triggered();

    //webSocket
    void onConnectedStatusChanged(bool status);
    void onDisplayError(const QString& what, const QString& err);
    void onGetActiveUsers(const QString& resp);
    void onGetGroupList(const QString& resp);
    void onFillUsers(const QString& resp);
    void onAddGroup(const QString& resp);
    void onEditGroup(const QString& resp);
    void onRemoveGroup(const QString& resp);
    void onAddUser(const QString& resp);
    void onDeleteUser(const QString& resp);
    void onUpdateUser(const QString& resp);
    void onSetUserParent(const QString& resp);
    void onClientLeave(const QString& resp);
    void onClientJoin(const QString& resp);

    void on_treeSrvObjects_itemSelectionChanged();
    void on_treeChannels_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_btnAddGroup_clicked();

    void on_btnEditGroup_clicked();

    void on_btnDelGroup_clicked();

    void on_treeChannels_itemActivated(QTreeWidgetItem *item, int column);

    void on_btnAddUser_clicked();

    void on_btnEditUser_clicked();

    void on_btnDeleteUser_clicked();

    void on_listChildSrvObjects_itemActivated(QTableWidgetItem *item);

    void on_btnViewMode_toggled(bool checked);

    void on_btnToGroup_clicked();

    void on_btnKillSession_clicked();

    void on_mnuAbout_triggered();

private:
    Ui::MainWindow *ui;
    ClientSettings settings;

    QLabel* lblStatusService;
    QLabel* lblStatusSocket;
    bWebSocket* client;
    bool isServiceStarted;

    QTreeWidget* treeServerObjects;
    QTableWidget * listChildServerObjects;
    QTreeWidget * treeChannelsObjects;

    QString currentNode;
    QMap<QString, int> treeGroupHeader;
    bool view_mode_hierarchy;

    //form
    void fillTree(bool started);
    void fillList(const QString &nodeName);
    QString serverView();
    void visibilityСontrol(bool visible, bool isSessions);
    void resizeColumns();
    void setHeaderAliases(QTableWidget* table);
    void treeGroupCreateColumns(QMap<QString, int> header, QTreeWidget* tree);
    void treeGroupCreateRootItems(QSortFilterProxyModel* model, QTreeWidget* tree, QMap<QString, int> header);
    void fillTreeGroup(QSortFilterProxyModel* model, QTreeWidgetItem* root, QMap<QString, int> header);

    std::string user_change_request_parameters(Ui::user_info *usr_info);
};

#endif // MAINWINDOW_H
