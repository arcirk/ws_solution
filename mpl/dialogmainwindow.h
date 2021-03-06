#ifndef DIALOGMAINWINDOW_H
#define DIALOGMAINWINDOW_H

#include <QDialog>
#include <userprofile.h>
#include <QMap>
#include <QUuid>
#include <QLineEdit>
#include "profilemanager.h"
#include <QTableWidgetItem>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QProcess>
#include <QDir>
#include <qmlwebsocket.h>
#include <QSqlDatabase>
#include <QLabel>
#include <sqlinterface.h>
#include <user.h>
#include <httpservice.h>
#include <QQueue>
#include <qjsontablemodel.h>
#include <tabledelegate.h>
#include "dialogclientoptions.h"
#include "mplsettings.h"
#include <QTimer>

#define ToDatabase                      "toDatabase"
#define ToRegistry                      "toRegistry"
#define ToVolume                        "toVolume"
#define ToRemoteVolume                  "ToRemoteVolume"
#define ToRemoteRegistry                "ToRemoteRegistry"
#define FromDatabase                    "fromDatabase"
#define FromRegistry                    "fromRegistry"

#define OBJECT_TYPE_CONTAINER           "Container"
#define OBJECT_TYPE_CERTIFICATE         "Certificate"
#define STORGARE_DATABASE               "StorgareDatabase"
#define STORGARE_LOCALHOST              "StorgareLocalhost"
#define STORGARE_REMOTEHOST             "StorgareRemotehost"

typedef std::function<void()> async_await;

namespace Ui {
class DialogMainWindow;
}

class DialogMainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMainWindow(QWidget *parent = nullptr);
    ~DialogMainWindow();

    void accept() override;
    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnAdd_clicked();

    void onSelectProfile();
    void onSelectDefaultAddress();
    void onSelectDefaultCertificate();

    void on_btnEdit_clicked();

    void on_btnDelete_clicked();

    void onTrayTriggered();
    void onAppExit();
    void onWindowShow();
    void onCheckIP();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayShowMessage(const QString& msg, int isError = false);
    void trayMessageClicked();

    void onConnectionSuccess();
    void onCloseConnection();
    void onConnectedStatusChanged(bool status);
    void onMessageReceived(const QString& msg, const QString& uuid, const QString& recipient, const QString& recipientName);
    void onDisplayError(const QString& what, const QString& err);
    void onWsExecQuery(const QString& result);
    void onWsGetAvailableContainers(const QString& recipient);
    void onWsGetInstalledCertificates(const QString& recipient);
    void onWsCommandToClient(const QString& recipient, const QString &command, const QString& message);

    void onParseCommand(const QVariant& result, int command);
    void onCommandError(const QString& result, int command);
    void onOutputCommandLine(const QString& data, int command);
    void onCommandLineStart();

    void onInstallCertificate();

    void on_btnSettings_clicked();

    void onGetCryptData(const QString& recipient);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void setProfoleImage(int index, const QString& imagePath);

    void on_btnUp_clicked();

    void on_btnDown_clicked();

    void on_deadline();

public slots:

private:

    Ui::DialogMainWindow *ui;

    QQueue<async_await> m_async_await;

    QJsonTableModel * modelMplProfiles;

    ProfileManager * _profiles;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *quitAction;
    QAction *showAction;
    QAction *checkIpAction;
    QAction *openFiefox;
    QAction *installCertAction;
    QProcess * mozillaApp;
    QDir appHome;
    QList<QVariant> lastParam;

    SqlInterface * db;
    QLabel * infoBar;
    CertUser * currentUser;
    bWebSocket* m_client;
    QString currentRecipient;

    QString _cprocsp_exe;
    QString _cprocsp_dir;
    QString _launch_mode;
    CommandLine * terminal;

    QList<QString> lastResult;
    QMap<QString, QString> m_colAliases;

    QTimer * deadline;


    void startMozillaFirefox();
    void currentUserSid();
    void currentUserGetConteiners();
    void currentUserGetCertificates();
    void getSettingsFromHttp();
    void connectToWsServer();
    void connectToDatabase();
    void csptestCurrentUserGetContainers(const QString &result);
    void createWsObject();
    void setWsConnectedSignals();
    void initProfiles();
    void setProfilesModel();
    void client_data_cghanged();
    void sendResultToClient();

    void asyncAwait();

    void createTerminal();
    void initCsptest();

    const QString _bankClientFile = "sslgate.url";
    const QString _bakClientUsbKey = "BankUsbKey";

    void updateData();
    void updateFromData();
    void setRow(UserProfile * prof);
    void updateRow(UserProfile * prof, int row);

    void setProfile(UserProfile* prof);

    void formControl();
    void createTrayActions();
    void createTrayIcon();
    void createDynamicMenu();

    QString toBankClientFile();

    ///////////////////////////

    void createColumnAliases();

    void updateConnectionStatus();
    void createConnectionsObjects();

    //////////////////////////


    /////////////////////////
    //const QString defaultHttpaddrr = "http://192.168.10.12/trade/hs/http_trade";

    /////
    void sendToRecipient(const QString &recipient, const QString &command, const QString &message, bool to_agent = false);

    void addContainer(const QString& from = "", const QString& to = "", const QString& byteArrayBase64 = "", const QString& ref = "");
    void addCertificate(const QString& from = "", const QString& to = "", const QString& byteArrayBase64 = "", const QString& ref = "", const QString& cntName = "", const QString& cntByteArrayBase64 = "");
    bool deleteLocalObject(const QString& objectName, const QString& objectType);

    void getDatabaseData(const QString& table, const QString& ref, const QJsonObject& param);
    void onGetDataFromDatabase(const QString &table, const QString param);
    //void getDatabaseCache(const QString& table, const QString& ref, const QJsonObject& param);

    void updateDataUserCache();
    void setFromDataUserCache(const QJsonObject& resp);
    void getUserData(bool resetCache = false);
    void get_user_data();

    void getAvailableCerts();
    void setAvailableCerts(const QJsonObject& resp);

    void updateTableImages();

    QStringList mozillaProfiles();
    void openMozillaFirefox();

    void startDeadline(){
        deadline->start(1000 * 60);
    };

signals:

};

#endif // DIALOGMAINWINDOW_H
