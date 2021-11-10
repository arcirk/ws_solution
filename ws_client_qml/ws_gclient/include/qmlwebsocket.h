#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "clientsettings.h"
#include "serveresponse.h"
#include "usersmodel.h"
//#include "qmlmessage.h"

class bWebSocket : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(bool started READ isStarted);
    Q_PROPERTY(QString user READ getUserName WRITE setUserName NOTIFY userChanged)
    Q_PROPERTY(QString hash READ getHash);
    Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool started READ isStarted NOTIFY startedChanged)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)
    Q_PROPERTY(bool saveHash READ saveHash WRITE setSaveHash NOTIFY saveHashChanged)
    Q_PROPERTY(bool pwdEdit READ pwdEdit WRITE setPwdEdit NOTIFY pwdEditChanged)
    Q_PROPERTY(UsersModel* catalog READ catalog WRITE setCatalog NOTIFY catalogChanged)
    //Q_PROPERTY(QString activePage READ getActivePage WRITE setActivePage);

public:
    explicit bWebSocket(QObject *parent = nullptr);
    ~bWebSocket();

    Q_INVOKABLE void open(const QString& user_name, const QString& user_password);
    Q_INVOKABLE void close();

    void ext_message(const std::string& msg);

    void processServeResponse(const QString& jsonResp);

    ClientSettings * get_settings();

    const QString getUserName();
    void setUserName(const QString& name);
    const QString getHash();
    bool isStarted();
    const QString getUserUUID();

//    const QString getActivePage();
//    void setActivePage(const QString& page);

    void getMessages(const QString& uuid_sub, int start_date, int end_date, int limit = 100, const QString& uuid_form = "");

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

    void setCatalog(UsersModel* model);
    UsersModel* catalog();

private:
    IClient * client;
    ClientSettings * settings;
    bool _pwdEdit;
    QString user;
    QString hash;
    UsersModel* m_usersCatalogModel;

signals:
    void displayError(const QString& what, const QString& err);
    void qmlError(const QString& what, const QString& err);
    void displayNotify(const QString& msg);
    //void qmlMessage(const QmlMessage& msg);
    void connectionSuccess();
    void closeConnection();
    void user_catalog(const QString& msg);
    void get_messages(const QString& resp);
    void nextChatPage(const QString& pageName, const QString& presentation);
    void setPage(int index);

    void userChanged();
    void hostChanged();
    void portChanged();
    void startedChanged();
    void autoConnectChanged();
    void saveHashChanged();
    void pwdEditChanged();
    void catalogChanged();
};

#endif // QMLWEBSOCKET_H
