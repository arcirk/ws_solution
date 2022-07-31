#include "websocket.h"
#include <QJsonDocument>
#include <QJsonObject>

#ifdef USE_ANDROID_SERVICE
#include <QAndroidJniEnvironment>
#include <QAndroidIntent>
#endif
#include <QtDebug>

#ifdef USE_ANDROID_SERVICE
WebSocketClient *WebSocketClient::m_instance = nullptr;

static void receivedFromAndroidService(JNIEnv *env, jobject /*thiz*/, jstring value)
{
    emit WebSocketClient::instance()->messageFromService(
        env->GetStringUTFChars(value, nullptr));
}
#endif

WebSocketClient::WebSocketClient(const QUrl& url, QObject *parent)
    : QObject{parent},
    m_url(url)
{
    m_started = false;

    m_client = new QWebSocket();

#ifdef USE_ANDROID_SERVICE
    m_instance = this;
    registerNatives();
    registerBroadcastReceiver();
#endif
    connect(m_client, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);

    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [=](QAbstractSocket::SocketError error){ onError(error); });

}

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject{parent}
{
    m_started = false;

    m_client = new QWebSocket();

#ifdef USE_ANDROID_SERVICE
    m_instance = this;
    registerNatives();
    registerBroadcastReceiver();
#endif
    connect(m_client, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);

    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            [=](QAbstractSocket::SocketError error){ onError(error); });
}

WebSocketClient::~WebSocketClient()
{
    if(isStarted())
        m_client->close();
}

void WebSocketClient::open()
{
    start();
}

void WebSocketClient::close()
{
    if(m_client)
        m_client->close();
}

void WebSocketClient::open(const QString &name, const QString &pass, const QString& app_name)
{

    if(!m_url.isValid()){
        emit displayError("Error", "Не задан адрес сервера!");
        return;
    }

    m_name = name;
    if(!pass.isEmpty())
        setPassword(pass);

    m_app_uuid = QUuid::createUuid();
    m_app_name = app_name;

    start();

}

bool WebSocketClient::isStarted()
{
    //qDebug() << __FUNCTION__ << m_client->state();
    return m_client->state() == QAbstractSocket::ConnectedState;
}

void WebSocketClient::send_command(const QString &command, const QUuid &uuid_form, const QString &param)
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

void WebSocketClient::setName(const QString &value)
{
    m_name = value;
}

QString WebSocketClient::name() const
{
    return m_name;
}

void WebSocketClient::setAppName(const QString &value)
{
    m_app_name = value;
}

QString WebSocketClient::appName() const
{
    return m_app_name;
}

void WebSocketClient::setPassword(const QString &value)
{
    m_hash = arcirk::get_hash(m_name, value);
}

QUuid WebSocketClient::uuid() const
{
    return m_session_uuid;
}

QUuid WebSocketClient::user_uuid() const
{
    return m_user_uuid;
}

void WebSocketClient::setUrl(const QUrl &url)
{
    m_url = url;
}

QUrl WebSocketClient::url() const
{
    return m_url;
}

void WebSocketClient::start()
{
    qDebug() << __FUNCTION__;

    m_client->open(m_url);

}

void WebSocketClient::onConnected()
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

void WebSocketClient::onDisconnected()
{
    qDebug() << __FUNCTION__;
    emit disconnected();
}

void WebSocketClient::onTextMessageReceived(const QString &message)
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

void WebSocketClient::set_param(ServerResponse* resp) {

    m_session_uuid = QUuid::fromString(resp->uuid_session);
    m_user_uuid = QUuid::fromString(resp->uuid);

}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    qCritical() << __FUNCTION__ << error;
    //emit error(error);
}


void WebSocketClient::processServeResponse(const QString &jsonResp)
{

    auto resp = new ServerResponse(jsonResp);

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
#ifdef USE_ANDROID_SERVICE
void WebSocketClient::sendToService(const QString &name)
{
    QAndroidIntent serviceIntent(QtAndroid::androidActivity().object(),
                                "ru/arcirk/qrpay/websocketclient/WebSocketClient");
    serviceIntent.putExtra("name", name.toUtf8());
    QAndroidJniObject result = QtAndroid::androidActivity().callObjectMethod(
                "startService",
                "(Landroid/content/Intent;)Landroid/content/ComponentName;",
                serviceIntent.handle().object());
}

void WebSocketClient::registerNatives()
{
    JNINativeMethod methods[] {
        {"sendToQt", "(Ljava/lang/String;)V", reinterpret_cast<void *>(receivedFromAndroidService)}};
    QAndroidJniObject javaClass("ru/arcirk/qrpay/websocketclient/ActivityUtils");

    QAndroidJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);
}

void WebSocketClient::registerBroadcastReceiver()
{
    QAndroidJniEnvironment env;
    jclass javaClass = env.findClass("ru/arcirk/qrpay/websocketclient/ActivityUtils");
    QAndroidJniObject classObject(javaClass);

    classObject.callMethod<void>("registerServiceBroadcastReceiver",
                                 "(Landroid/content/Context;)V",
                                 QtAndroid::androidContext().object());
}
#endif
