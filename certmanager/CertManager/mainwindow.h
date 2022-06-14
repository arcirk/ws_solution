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
#include <qproxymodel.h>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define SqlContainers                   "SqlContainers"
#define SqlCertificates                 "SqlCertificates"
#define DataCertificatesList            "DataCertificatesList"
#define DataContainersList              "DataContainersList"
#define DataUsersList                   "DataUsersList"
#define SqlServer                       "SqlServer"
#define SqlUsers                        "SqlUsers"
#define currentUserRegistry             "currentUserRegistry"
#define currentUserCertificates         "currentUserCertificates"
#define currentUserContainers           "currentUserContainers"
#define currentUserDivace               "currentUserDivace"
#define currentUserAvailableContainers  "currentUserAvailableContainers"
#define insertCertificateToData         "insertCertificateToData"
#define deleteContainerFromData         "deleteContainerFromData"
#define deleteCertificateFromData       "deleteCertificateFromData"
#define deleteUserFromData              "deleteUserFromData"
#define insertContainerToData           "insertContainerToData"
#define insertUserToData                "insertContainerToData"
#define WsActiveUsers                   "WsActiveUsers"

//Команды клиенту
#define AvailableContainers             "available_containers"
#define GetAvailableContainers          "get_available_containers"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_mnuExit_triggered();

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
    void onWsExecQuery(const QString& result);

    void onOutputCommandLine(const QString& data, int command);
    void onParseCommand(const QVariant& result, int command);
    void onCommandError(const QString& result, int command);

    void onWsGetAvailableContainers(const QString& recipient);
    void onWsCommandToClient(const QString& recipient, const QString &command, const QString& message);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

    void on_btnEdit_clicked();


    void on_actionTest_triggered();

    void on_btnTermOptions_clicked();

    void on_btnTerminalClear_clicked();

    void on_btnCompToDatabase_clicked();

    void on_btnUserToDatabase_clicked();

    void on_btnDatabaseAdd_clicked();

    void on_btnSendCommand_clicked();

    void on_btnCurrentUserSaveAs_clicked();

    void on_btnCurrentCopyToRegistry_clicked();

    void on_btnCurrentCopyToSql_clicked();

    void on_btnDatabaseDelete_clicked();

    void on_btnConInfo_clicked();

    void on_btnCurrentDelete_clicked();

    void on_btnDatabaseSaveAs_clicked();

    void updateInfoContainerOnDatabase(const QString& info, const QString& name, const QString& nameBase64, KeysContainer* cnt);

    void on_btnDatabaseInfo_clicked();

    void on_btnCurrentUserAdd_clicked();

    void on_mnuCertUsers_triggered();

    void on_mnuCryptoPro_triggered();

private:
    Ui::MainWindow *ui;
    QList<QToolButton*> toolBar;
    QList<QToolButton*> toolBarActiveUsers;
    Settings * _sett;

    QJsonTableModel * modelUserContainers;
    QProxyModel     * proxyModelUserConteiners;
    QJsonTableModel * modelUserCertificates;
    QJsonTableModel * modelSqlContainers;
    QJsonTableModel * modelSqlCertificates;
    QJsonTableModel * modelWsUsers;
    QJsonTableModel * modelSqlUsers;

    QString currentRecipient;
    void sendToRecipient(const QString &recipient, const QString& command, const QString &message);

    void resetInfoUserContainers();

    void createModels();

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
    void connectToDatabase(Settings * sett, const QString& pwd);

    void createTerminal();

    void createTree();

    Certificate* selectCertFromLocalHost();

    void getDataContainersList();
    void getDataCertificatesList();
    void getDataUsersList();

    void resetCurrentUserCertModel();

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

    void treeSetCurrentContainers(const QString& filter);
    void treeSetFromSqlContainers();
    void treeSetFromSqlUsers();
    void treeSetFromSqlCertificates();
    void treeSetOnlineWsUsers();
    void updateContainerInfoOnData(const QString& info);
    void treeSetFromCurrentUserCerts();

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

    void saveAsCurrentUserContainer();
    void saveAsCurrentUserCertificate();
    void saveAsDatabaseContainer();
    void saveAsDatabaseCertificate();


    void addCertificate();
    void delCertificate();
    void addContainer();
    void delContainer();
    void delCertUser();
    void addCertUser();

    QStandardItemModel *getLocalMountedVolumes();

};
#endif // MAINWINDOW_H
