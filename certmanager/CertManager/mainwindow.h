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

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void onReconnect(settings * sett, const QString& pwd);

    void on_mnuConnect_triggered();

    void on_btnAdd_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_btnToDatabase_clicked();

    void on_btnDelete_clicked();

    void on_btnInstallToUser_clicked();

    void on_mnuOptions_triggered();

    void onConnectionSuccess();
    void onCloseConnection();
    void onConnectedStatusChanged(bool status);
    void onClientJoin(const QString& resp);
    void onClientLeave(const QString& resp);
    void onMessageReceived(const QString& msg, const QString& uuid, const QString& recipient, const QString& recipientName);
    void onDisplayError(const QString& what, const QString& err);

private:
    Ui::MainWindow *ui;
    QList<QToolButton*> toolBar;
    settings * _sett;
    QSqlDatabase db;
    QLabel * infoBar;
    CertUser * currentUser;
    QMap<QString, CertUser*> m_users;
    bWebSocket* m_client;

    void createTree();
    void createRootList();
    void loadContainersList();
    void LoadUsersList();
    void createCertList();
    void createUsersList();
    void enableToolbar(bool value);
    void loadItemChilds(QTreeWidgetItem *item);
    void loadItemSpecific(QTreeWidgetItem *item);
    void loadKeysOnRegistry(CertUser * usr);
    void disableToolBar();
    bool isContainerExists(const QString& name);
    bool isUserExists(const QString& name);
    void userToDatabase(const QString& name);

    void connectToWsServer();
};
#endif // MAINWINDOW_H
