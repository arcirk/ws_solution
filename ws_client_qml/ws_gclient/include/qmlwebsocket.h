#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "clientsettings.h"
#include "serveresponse.h"

class bWebSocket : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(bool started READ isStarted);
    Q_PROPERTY(QString user READ getUserName WRITE setUserName NOTIFY userChanged)
    Q_PROPERTY(QString hash READ getHash);
    Q_PROPERTY(QString uuidUser READ getUserUUID);
    Q_PROPERTY(QString appName READ getAppName);
    Q_PROPERTY(QString uuidSession READ getUuidSession);
    Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool started READ isStarted NOTIFY startedChanged)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)
    Q_PROPERTY(bool saveHash READ saveHash WRITE setSaveHash NOTIFY saveHashChanged)
    Q_PROPERTY(bool pwdEdit READ pwdEdit WRITE setPwdEdit NOTIFY pwdEditChanged)
    Q_PROPERTY(bool connectedStatus READ connectedStatus NOTIFY connectedStatusChanged)
    //Q_PROPERTY(QString activePage READ getActivePage WRITE setActivePage);

public:
    explicit bWebSocket(QObject *parent = nullptr);
    ~bWebSocket();

    Q_INVOKABLE void open(const QString& user_name, const QString& user_password);
    Q_INVOKABLE void close();
    Q_INVOKABLE void saveCache(const QString& jsonText);
    Q_INVOKABLE void messages(const QString& uuid);
    Q_INVOKABLE void sendMessage(const QString& recipient, const QString& msg);
    Q_INVOKABLE void getUserInfo(const QString& uuid);
    Q_INVOKABLE void getUserStatus(const QString& uuid, const QString& param);

    void ext_message(const std::string& msg);
    void status_changed(bool status);

    void processServeResponse(const QString& jsonResp);

    ClientSettings * get_settings();

    const QString getUserName();
    void setUserName(const QString& name);
    const QString getHash();
    bool isStarted();
    const QString getUserUUID();
    const QString getUuidSession();
    const QString getAppName();
    void get_messages(const QString& uuid_sub, int start_date, int end_date, int limit = 100, const QString& uuid_form = "");

    static long int currentDate();
    static long int addDay(const long int source, const int dayCount);

    void setHost(const QString& newHost);
    const QString& getHost();
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

private:
    IClient * client;
    ClientSettings * settings;
    bool _pwdEdit;
    QString user;
    QString hash;

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
};

#endif // QMLWEBSOCKET_H
