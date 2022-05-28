#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "clientsettings.h"
#include "serveresponse.h"

#include "qwebdav.h"


class bWebSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString user READ getUserName WRITE setUserName NOTIFY userChanged)
    Q_PROPERTY(QString hash READ getHash);
    Q_PROPERTY(QString uuidUser READ getUserUUID);
    Q_PROPERTY(QString appName READ getAppName WRITE setAppName NOTIFY appNameChanged);
    Q_PROPERTY(QString uuidSession READ getUuidSession);
    Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool started READ isStarted NOTIFY startedChanged)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)
    Q_PROPERTY(bool saveHash READ saveHash WRITE setSaveHash NOTIFY saveHashChanged)
    Q_PROPERTY(bool pwdEdit READ pwdEdit WRITE setPwdEdit NOTIFY pwdEditChanged)
    Q_PROPERTY(bool connectedStatus READ connectedStatus NOTIFY connectedStatusChanged)
    Q_PROPERTY(QString settingsFileName READ getSettingsFileName WRITE setSettingsFileName NOTIFY settingsFileNameChanged);
    Q_PROPERTY(bool isAgentUse READ isAgentUse)
    Q_PROPERTY(bool isAgent READ isAgent);
    Q_PROPERTY(bool hidden READ isHidden WRITE setHidden NOTIFY hiddenChanged);

public:
    explicit bWebSocket(QObject *parent = nullptr, const QString& confFile = "");
    ~bWebSocket();

    static QString generateHash(const QString& usr, const QString& pwd);

    Q_INVOKABLE void open(const QString& user_name, const QString& user_password);
    Q_INVOKABLE void close();
    Q_INVOKABLE void saveCache(const QString& jsonText);
    Q_INVOKABLE void messages(const QString& uuid);
    Q_INVOKABLE void sendMessage(const QString& recipient, const QString& msg, const QString& objectName = "", const QString& msg_ref = "");
    Q_INVOKABLE void getUserInfo(const QString& uuid);
    Q_INVOKABLE void getUserStatus(const QString& uuid);
    Q_INVOKABLE void getUserData(const QString& uuid, const QString& param);
    Q_INVOKABLE void resetUnreadMsgFromData(const QString& sender);


    Q_INVOKABLE void getUsers(const QString& parent);
    Q_INVOKABLE void addGroup(const QString& name, const QString& presentation, const QString& parent);
    Q_INVOKABLE void editGroup(const QString& uuid, const QString& name, const QString& presentation, const QString& parent);
    Q_INVOKABLE void removeGroup(const QString& uuid);

    Q_INVOKABLE void sendCommand(const QString& cmd, const QString& uuidForm = "", const QString& param = "");
    Q_INVOKABLE void sendCommandToAgent(const QString& cmd, const QString& msg);

    //agent
    Q_INVOKABLE void agentClientShow();
    Q_INVOKABLE void registerToAgent(const QString& uuid);
    Q_INVOKABLE void setUuidSessAgent(const QString& uuid);
    //Q_INVOKABLE void registerClientForAgent(const QString& uuid);

    //webdaw
    Q_INVOKABLE void uploadFile(const QString& token, const QString& fileName, const QString &ref);
    Q_INVOKABLE void
    downloadFile(const QString &token, const QString &fileName, const QString &ref, const QString &outputDir);
    Q_INVOKABLE bool verifyLocalRoomCacheDirectory(const QString& roomToken);
    Q_INVOKABLE bool saveFileToUserCache(const QString& token, const QString& localFile, const QString& refMessage);
    Q_INVOKABLE QString getFileName(const QString& filePath);
    //
    Q_INVOKABLE QString getObjectHtmlSource(const QString& fileName);
    Q_INVOKABLE bool isImage(const QString& fileName);
    Q_INVOKABLE bool fileExists(const QString& path);
    Q_INVOKABLE QString getRandomUUID();
    Q_INVOKABLE QString separator();

    void ext_message(const std::string& msg);
    void status_changed(bool status);

    void processServeResponse(const QString& jsonResp);

    ClientSettings & options();

    [[nodiscard]] QString getUserName() const;
    void setUserName(const QString& name);
    [[nodiscard]] QString getHash() const;

    bool isStarted();
    const QString getUserUUID();
    const QString getUuidSession();
    const QString getAppName();
    void setAppName(const QString& name);

    void get_messages(const QString& uuid_sub, int start_date, int end_date, int limit = 100, const QString& uuid_form = "");

    static long int currentDate();
    static long int addDay(const long int source, const int dayCount);

    void setHost(const QString& newHost);
    QString getHost();
    void setPort(int newPort);
    int getPort();

    //bool getStarted();

    bool autoConnect();
    void setAutoConnect(bool value);

    bool saveHash();
    void setSaveHash(bool value);
    bool pwdEdit();
    void setPwdEdit(bool value);

    bool connectedStatus();

    void setSettingsFileName(const QString& fname);
    QString getSettingsFileName();

    void updateSettings();

    static const QString nilUuid();
    static const QString randomUuid();

    void removeUser(const QString& uuid);
    void setParent(const QString& uuid, const QString& parent);
    void killSession(const QString& uuid);

    void setWebDavSettingsToClient(const QString& resp);
    void setWebDavSettingsToServer();

    void setSqlOptions();

    bool isAgentUse();
    bool isAgent();
    void setIsAgent(bool val);
    bool isHidden();
    void setHidden(bool val);

    static std::string get_std_parent_path();

    void setOsUserName(const QString& value);

//    static std::string string_utf_to_utf(const std::string &s);
//    static std::string string_to_utf(const std::string &s, const char* charset);
//    static std::string string_from_utf(const std::string &s, const char* charset);

private:
    IClient * client;
    ClientSettings settings;
    bool _pwdEdit;
    QString user;
    QString hash;
    QString confFileName;
    QString os_user_name;

    QMap<QString,QString> m_agentClients;
    void joinClientToAgent(ServeResponse * resp);
    QStringList getImageMimeType();

    QWebdav * qWebdav;

    bool m_is_agent;
    bool m_hidden;
    QString uuidSessionAgent;

    void responseCommand(ServeResponse * resp);

    //QString osUserName();

signals:
    void displayError(const QString& what, const QString& err);
    void qmlError(const QString& what, const QString& err);
    void displayNotify(const QString& msg);
    //void qmlMessage(const QmlMessage& msg);
    void connectionSuccess();
    void closeConnection();
    void user_catalog(const QString& msg);
    void setMessages(const QString& resp);
    void nextChatPage(const QString& pageName, const QString& presentation);
    void setPage(int index);

    void userChanged();
    void hostChanged();
    void portChanged();
    void startedChanged();
    void autoConnectChanged();
    void saveHashChanged();
    void pwdEditChanged();
    void resetUsersCatalog(const QString& resp);
    void getUserCache(const QString& resp);
    void messageReceived(const QString& msg, const QString& uuid, const QString& recipient, const QString& recipientName);

    void connectedStatusChanged(bool status);
    void userInfo(const QString& uuid);
    void clientJoin(const QString& resp);
    void getActiveUsers(const QString& resp);
    void clientLeave(const QString& resp);
    void requestUserData(const QString& resp);
    void unreadMessages(const QString& resp);
    void requestUserStatus(const QString& resp);
    void settingsFileNameChanged();
    void getGroupList(const QString& resp);
    void getListUsers(const QString& resp);
    void addGroupUsers(const QString& resp);
    void editGroupUsers(const QString& resp);
    void removeGroupUsers(const QString& resp);

    void addUser(const QString& resp);
    void updateUser(const QString& resp);
    void deleteUser(const QString& resp);
    void setUserParent(const QString& resp);

    void clientShow();

    void getChannelToken(const QString& resp);

    void appNameChanged();

    void setProgress(double value);
    void webDavError(const QString& err);

    void hiddenChanged(bool value);

private slots:
    void onWebDavError(QNetworkReply::NetworkError type, const QString& error);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

public slots:
    void downloadFinished();
    void uploadFinished();
    void downloadError();
    void uploadError();

};

#endif // QMLWEBSOCKET_H
