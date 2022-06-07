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
#include <qjsontablemodel.h>

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

    void connectToDatabase(Settings * sett, const QString& pwd);

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
    void onClientJoinEx(const QString& resp, const QString& ip_address, const QString& host_name, const QString& app_name, const QString& user_name);
    void onClientLeave(const QString& resp);
    void onMessageReceived(const QString& msg, const QString& uuid, const QString& recipient, const QString& recipientName);
    void onDisplayError(const QString& what, const QString& err);
    void UpdateRowIcons();
    void onGetActiveUsers(const QString& resp);
    void onParseCommand(const QString& result, int command);
    void onCommandError(const QString& result, int command);
    void onWsExecQuery(const QString& result);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

    void on_btnEdit_clicked();

    void onOutputCommandLine(const QString& data, int command);
    void on_actionTest_triggered();

    void on_btnTermOptions_clicked();

    void on_btnTerminalClear_clicked();

    void on_btnCompToDatabase_clicked();

    void on_btnUserToDatabase_clicked();

    void on_btnContAdd_clicked();

    void on_btnSendCommand_clicked();

    void on_btnCurrentCopyToDisk_clicked();

    void on_btnCurrentCopyToRegistry_clicked();

    void on_btnCurrentCopyToSql_clicked();

    void on_btnConDel_clicked();

    void on_btnConInfo_clicked();

    void on_btnCurrentDelete_clicked();

    void on_btnCopyToDiskFromDatabase_clicked();

private:
    Ui::MainWindow *ui;
    QList<QToolButton*> toolBar;
    QList<QToolButton*> toolBarActiveUsers;
    Settings * _sett;

    QJsonTableModel * modelSqlContainers;
    QJsonTableModel * modelSqlCertificates;
    QJsonTableModel * modelWsUsers;
    QJsonTableModel * modelSqlUsers;

    SqlInterface * db;
    QLabel * infoBar;
    CertUser * currentUser;
    QMap<QString, CertUser*> m_users;
    bWebSocket* m_client;

    CommandLine * terminal;

    QString _cprocsp_exe;
    bool isUseCsptest;
    QString _cprocsp_dir;
    QString _launch_mode;

    QMap<QUuid, CertUser*> m_actUsers;
    QMap<QString, QString> m_colAliases;

    void createColumnAliases();

#ifdef _WINDOWS
    void execute_command(QString param);
#endif
    void createConnectionsObjects();

    void connectToWsServer();
    void createWsObject();
    void setWsConnectedSignals();

    void createTerminal();

    void createTree();
    void createRootList();

    void getDataContainersList();
    void getDataCertificatesList();
    void getDataUsersList();

    void LoadUsersList();
    void loadCertList();
    void createUsersList();
    void enableToolbar(bool value);
    void loadItemChilds(QTreeWidgetItem *item);
    void loadItemSpecific(QTreeWidgetItem *item);
    void getAvailableContainers(CertUser * usr);
    void loadOnlineUsers();
    void disableToolBar();
    bool isContainerExists(const QString& name);
    bool isCertUserExists(const QString& name, const QString& host = "");
    bool isWsUserExists(const QString& name, const QString& host = "");
    bool isHostExists(const QString& name);
    void userToDatabase(const QString& name);
    void loadCimputers();

    void toolBarSetVisible(QWidget * bar, bool value);

    void inVisibleToolBars();
    void setKeysToRegistry();
    bool isDbOpen();
    void formControl();
    void initCsptest();

    void csptestCurrentUserGetContainers(const QString& result);

    QTreeWidgetItem * addTreeNode(const QString &text, const QVariant &key, const QString &imagePath);
    QTreeWidgetItem * findTreeItem(const QString& key);
    QTreeWidgetItem * findTreeItem(const QString& key, QTreeWidgetItem * parent);

    void treeSetCurrentContainers(QStringList data);
    void treeSetFromSqlContainers();
    void treeSetFromSqlUsers();
    void treeSetFromSqlCertificates();
    void treeSetOnlineWsUsers();
    void updateContainerInfoOnData(const QString& info);

    void resetTableJsonModel(const QJsonObject& obj, const QString& cmd);

    QModelIndex findInTable(QAbstractItemModel * model, const QString& value, int column, bool findData = true);

    QString validUuid(const QString& uuid);

    QString fromBase64(const QString& value);

    void onGetDataFromDatabase(const QString& table, const QString param);
//    static inline bool byte_is_base64(unsigned char c) {
//        return (isalnum(c) || (c == '+') || (c == '/'));
//    }

    bool isCyrillic(const QString& source);
    QJsonObject parseDeviceString(const QString& key);
};
#endif // MAINWINDOW_H
