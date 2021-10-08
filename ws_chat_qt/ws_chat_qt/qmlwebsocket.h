#ifndef QMLWEBSOCKET_H
#define QMLWEBSOCKET_H

#include <QObject>
#include <iws_client.h>
#include "appsettings.h"
#include "serveresponse.h"

class QmlWebSocket : public QObject
{
    Q_OBJECT
public:
    explicit QmlWebSocket(QObject *parent = nullptr);
    ~QmlWebSocket();

    Q_INVOKABLE void open(const QString& user_name, const QString& user_password);

    void ext_message(const std::string& msg);

    void processServeResponse(const QString& jsonResp);

    appSettings * get_settings();

private:
    IClient * client;
    appSettings * settings;



signals:
    void display_error(const QString& what, const QString& err);
};

#endif // QMLWEBSOCKET_H
