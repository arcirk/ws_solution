#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "appsettings.h"
#include "serveresponse.h"
#include "qmlmessage.h"

class QmlWebSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool started READ isStarted);
    Q_PROPERTY(QString user READ getUserName WRITE setUserName)
    Q_PROPERTY(QString hash READ getHash);
    Q_PROPERTY(QString activePage READ getActivePage WRITE setActivePage);

public:
    explicit QmlWebSocket(QObject *parent = nullptr);
    ~QmlWebSocket();

    Q_INVOKABLE void open(const QString& user_name, const QString& user_password);

    void ext_message(const std::string& msg);

    void processServeResponse(const QString& jsonResp);

    appSettings * get_settings();

    const QString getUserName();
    void setUserName(const QString& name);
    const QString getHash();
    bool isStarted();

    const QString getActivePage();
    void setActivePage(const QString& page);

private:
    IClient * client;
    appSettings * settings;
    QString activePage;
    QString user;
    QString hash;


signals:
    void display_error(const QString& what, const QString& err);
    void qmlError(const QString& what, const QString& err);
    void display_notify(const QString& msg);
    void qmlMessage(const QmlMessage& msg);
    void connectionSuccess();
    void user_catalog(const QString& msg);
    void nextChatPage(const QString& pageName, const QString& presentation);
    //void chatPage(const QString& msg);
    void setPage(int index);
};

#endif // QMLWEBSOCKET_H
