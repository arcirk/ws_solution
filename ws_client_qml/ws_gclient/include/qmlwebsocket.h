#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "clientsettings.h"
#include "serveresponse.h"
//#include "qmlmessage.h"

class bWebSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool started READ isStarted);
    Q_PROPERTY(QString user READ getUserName WRITE setUserName NOTIFY UserChanged)
    Q_PROPERTY(QString hash READ getHash);
    Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool started READ getStarted NOTIFY startedChanged)

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

    bool getStarted();

private:
    IClient * client;
    ClientSettings * settings;
    //QString activePage;
    QString user;
    QString hash;
    //QString _host;
    //int _port;


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

    void UserChanged();
    void hostChanged();
    void portChanged();
    void startedChanged();
};

#endif // QMLWEBSOCKET_H
