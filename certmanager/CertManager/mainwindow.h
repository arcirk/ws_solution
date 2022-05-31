#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QToolButton>
#include "settings.h"
#include <QSqlDatabase>
#include <QLabel>
#include "user.h"
#include "dialogselectdevice.h"
#include "keyscontainer.h"
#include <qmlwebsocket.h>
#include <commandline.h>
#include <sqlinterface.h>
#include <QUuid>

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

    void onReconnect(settings * sett, const QString& pwd);

    void on_mnuConnect_triggered();

    void on_btnAdd_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btnToDatabase_clicked();

    void on_btnDelete_clicked();

    void on_btnInstallToUser_clicked();

    void on_mnuOptions_triggered();

    void onConnectionSuccess();
    void onCloseConnection();
    void onConnectedStatusChanged(bool status);
    void onClientJoinEx(const QString& resp, const QString& ip_address, const QString& host_name, const QString& app_name);
    void onClientLeave(const QString& resp);
    void onMessageReceived(const QString& msg, const QString& uuid, const QString& recipient, const QString& recipientName);
    void onDisplayError(const QString& what, const QString& err);
    void UpdateRowIcons();
    void onGetActiveUsers(const QString& resp);
    void onParseCommand(const QString& result, CommandLine::cmdCommand command);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

    void on_btnEdit_clicked();

    void onOutputCommandLine(const QString& data, CommandLine::cmdCommand command);
    void on_actionTest_triggered();

    void on_btnTermOptions_clicked();

    void on_btnTerminalClear_clicked();

    void on_btnCompToDatabase_clicked();

    void on_btnUserToDatabase_clicked();

    void on_btnContAdd_clicked();

private:
    Ui::MainWindow *ui;
    QList<QToolButton*> toolBar;
    QList<QToolButton*> toolBarActiveUsers;
    settings * _sett;
    //QSqlDatabase db;
    SqlInterface * db;
    QLabel * infoBar;
    CertUser * currentUser;
    QMap<QString, CertUser*> m_users;
    bWebSocket* m_client;

    CommandLine * terminal;

    QMap<QUuid, CertUser*> m_actUsers;

    void execute_command(QString param);

    void createTree();
    void createRootList();
    void loadContainersList();
    void LoadUsersList();
    void loadCertList();
    void createUsersList();
    void enableToolbar(bool value);
    void loadItemChilds(QTreeWidgetItem *item);
    void loadItemSpecific(QTreeWidgetItem *item);
    void loadKeysOnRegistry(CertUser * usr);
    void loadOnlineUsers();
    void disableToolBar();
    bool isContainerExists(const QString& name);
    bool isCertUserExists(const QString& name, const QString& host = "");
    bool isHostExists(const QString& name);
    void userToDatabase(const QString& name);
    void loadCimputers();
    void connectToWsServer();
    void toolBarSetVisible(QWidget * bar, bool value);
    void createWsObject();
    void setConnectedSignals();
    void initToolBars();
    void setKeysToRegistry();
    bool isDbOpen();

};
#endif // MAINWINDOW_H
