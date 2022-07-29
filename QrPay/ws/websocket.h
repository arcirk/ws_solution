#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QUuid>
#include "arcirk.h"
#include <serveresponse.h>

class WebSocket : public QObject
{
    Q_OBJECT
public:
    explicit WebSocket(const QUrl& url, QObject *parent = nullptr);

    void open();
    void close();
    void open(const QString& name, const QString& pass = "", const QString& app_name = "unknown");

    bool isStarted();

    void send_command(const QString& command, const QUuid& uuid_form = {}, const QString& param = "");

    void setName(const QString& value);
    QString name() const;
    void setAppName(const QString& value);
    QString appName() const;
    void setPassword(const QString& value);
    QString hash() const;
    QUuid uuid() const;
    QUuid user_uuid() const;

private:
    QUrl m_url;
    QString m_name;
    QString m_hash;
    QString m_app_name;
    QUuid m_app_uuid;
    QUuid m_user_uuid;
    QString m_user_name;
    bool m_started;
    QString _client_param;
    QUuid m_session_uuid;

    QWebSocket* m_client;

    void set_param(ServeResponse* resp);
    void start();
    void processServeResponse(const QString &jsonResp);

private slots:

    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

signals:

    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void error(QAbstractSocket::SocketError error);
    void connectionSuccess(); //при успешной авторизации
    void displayError(const QString& what, const QString& err);

};

#endif // WEBSOCKET_H
