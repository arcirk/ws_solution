#include "websocket.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <message.h>

WebSocket::WebSocket(const QUrl& url, QObject *parent)
    : QObject{parent},
    m_url(url)
{
    m_started = false;

    m_client = new QWebSocket();

    connect(m_client, &QWebSocket::connected, this, &WebSocket::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocket::onDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocket::onTextMessageReceived);

    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [=](QAbstractSocket::SocketError error){ onError(error); });


#ifdef Q_OS_WINDOWS
    std::string envUSER = "username";
    QByteArray data(std::getenv(envUSER.c_str()));
    m_user_name = QString::fromLocal8Bit(data);
#else

#endif
}

void WebSocket::open()
{
    start();
}

void WebSocket::close()
{
    if(m_client)
        m_client->close();
}

void WebSocket::open(const QString &name, const QString &pass, const QString& app_name)
{
    m_name = name;
    if(!pass.isEmpty())
        setPassword(pass);

    m_app_uuid = QUuid::createUuid();
    m_app_name = app_name;

    start();

}

bool WebSocket::isStarted()
{
    //qDebug() << __FUNCTION__ << m_client->state();
    return m_client->state() == QAbstractSocket::ConnectedState;
}

void WebSocket::send_command(const QString &command, const QUuid &uuid_form, const QString &param)
{
    qDebug() << __FUNCTION__ << command;

    auto _msg = arcirk::Message();
    _msg.message().uuid =  m_app_uuid;
    _msg.message().message = param;
    _msg.message().name = m_name;
    _msg.message().app_name = m_app_name;
    _msg.message().command = command;
    _msg.message().uuid_form = uuid_form;
    _msg.message().user_name = m_user_name;

    QString msg = "cmd " + _msg.get_json(true);

    m_client->sendTextMessage(msg);

}

void WebSocket::setName(const QString &value)
{
    m_name = value;
}

QString WebSocket::name() const
{
    return m_name;
}

void WebSocket::setAppName(const QString &value)
{
    m_app_name = value;
}

QString WebSocket::appName() const
{
    return m_app_name;
}

void WebSocket::setPassword(const QString &value)
{
    m_hash = arcirk::get_hash(m_name, value);
}

QUuid WebSocket::uuid() const
{
    return m_session_uuid;
}

QUuid WebSocket::user_uuid() const
{
    return m_user_uuid;
}

void WebSocket::start()
{
    qDebug() << __FUNCTION__;

    m_client->open(m_url);

}

void WebSocket::onConnected()
{
    qDebug() << __FUNCTION__;
    emit connected();

    auto pt = QJsonObject();

    pt.insert("uuid", m_app_uuid.toString());
    pt.insert("name", m_name);
    pt.insert("hash", m_hash);
    pt.insert("app_name", m_app_name);
    pt.insert("user_uuid", m_user_uuid.toString());
    pt.insert("user_name", m_user_name);
    pt.insert("host_name", QSysInfo::machineHostName());


    _client_param = QJsonDocument(pt).toJson();

    send_command("set_client_param", m_app_uuid, _client_param);
}

void WebSocket::onDisconnected()
{
    qDebug() << __FUNCTION__;
    emit disconnected();
}

void WebSocket::onTextMessageReceived(const QString &message)
{
    qDebug() << __FUNCTION__;// << message;

    if (message == "\n" || message.isEmpty() || message == "pong")
        return;

    auto v = message.split("\n");

    QString msg;

    if (v.size() > 0)
        msg = v[0];


    if(msg.left(6) == "result"){
        QByteArray resp = QByteArray::fromBase64(msg.right(msg.length() - 7).toUtf8());
        processServeResponse(resp);
    }else
        emit messageReceived(message);
}

void WebSocket::set_param(ServeResponse* resp) {

    m_session_uuid = QUuid::fromString(resp->uuid_session);
    m_user_uuid = QUuid::fromString(resp->uuid);

}

void WebSocket::onError(QAbstractSocket::SocketError error)
{
    qCritical() << __FUNCTION__ << error;
    //emit error(error);
}


void WebSocket::processServeResponse(const QString &jsonResp)
{

    auto resp = new ServeResponse(jsonResp);

    if(!resp->isParse){
        delete resp;
        return;
    }

    if(resp->result == "error"){

            emit displayError(resp->command, resp->message);

            if(resp->command == "set_client_param")
                m_client->close();

    }else{
        if(resp->command == "set_client_param"){
            set_param(resp);
            emit connectionSuccess();
        }

    }

    delete resp;
}
