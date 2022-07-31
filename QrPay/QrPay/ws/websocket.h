#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#ifdef USE_ANDROID_SERVICE
#include <QtAndroid>
#include <QAndroidIntent>
#endif
#include <QtWebSockets/QWebSocket>
#include <QUuid>
#include "arcirk.h"
#include "serverresponse.h"

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:
    explicit WebSocketClient(const QUrl& url, QObject *parent = nullptr);
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void open(const QString& name, const QString& pass = "", const QString& app_name = "unknown");

    Q_INVOKABLE bool isStarted();

    Q_INVOKABLE void send_command(const QString& command, const QUuid& uuid_form = {}, const QString& param = "");

    void setName(const QString& value);
    QString name() const;
    void setAppName(const QString& value);
    QString appName() const;
    void setPassword(const QString& value);
    QString hash() const;
    QUuid uuid() const;
    QUuid user_uuid() const;

    void setUrl(const QUrl& url);
    QUrl url() const;

#ifdef USE_ANDROID_SERVICE
    static WebSocketClient *instance() { return m_instance; }
    Q_INVOKABLE void sendToService(const QString &name);
#endif
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
#ifdef USE_ANDROID_SERVICE
    static WebSocketClient *m_instance;
    void registerNatives();
    void registerBroadcastReceiver();
#endif
    void set_param(ServerResponse* resp);
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

    void urlChanged();

#ifdef USE_ANDROID_SERVICE
    void messageFromService(const QString &message);
#endif
};

#endif // WEBSOCKET_H
