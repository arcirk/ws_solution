#include "qmlwebsocket.h"

QmlWebSocket::QmlWebSocket(QObject *parent) : QObject(parent)
{

    settings = new appSettings();
    settings->init();

    _callback_message callback = std::bind(&QmlWebSocket::ext_message, this, std::placeholders::_1);
    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback);
}

QmlWebSocket::~QmlWebSocket()
{
    if (client->started()) {
        client->close();
    }

    delete client;
}

void QmlWebSocket::open(const QString& user_name, const QString& user_password)
{
    if (client->started()) {
        return;
    }

    if (!user_password.isEmpty()) {
        settings->Hash = QString::fromStdString( arc_json::get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    if (settings->Hash.isEmpty()) {
        settings->Hash = QString::fromStdString( arc_json::get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    settings->RootUser = user_name;

    client->admin_name = settings->RootUser.toStdString();
    client->hash = settings->Hash.toStdString();
    client->host = settings->ServerHost.toStdString();
    client->port = settings->ServerPort;

    client->open();

    settings->save_settings();
}

void QmlWebSocket::close()
{
    if(client->started()){
        client->close();
    }
}

void QmlWebSocket::ext_message(const std::string &msg)
{
    QString resp = ServeResponse::base64_decode(msg);
    //qDebug() << QString::fromStdString(msg);

    if(!resp.isEmpty()){
        processServeResponse(resp);
    }

}

void QmlWebSocket::processServeResponse(const QString &jsonResp)
{
    //qDebug() << jsonResp;
    auto resp = new ServeResponse(jsonResp);

    if(!resp->isParse){
        delete resp;
        return;
    }
    if(resp->result == "error"){

            emit display_error(resp->command, resp->message);
            emit qmlError(resp->command, resp->message);

            if(resp->command == "set_client_param")
                client->close();

    }else
    {
        if(resp->command == "set_client_param"){
            emit display_notify("Подключился к серверу.");
            emit connectionSuccess();
            client->get_users_catalog("");
        }else if (resp->command == "get_users_catalog"){
            //qDebug() << resp->message;
            emit user_catalog(resp->message);
        }else if (resp->command == "get_messages"){
            //qDebug() << resp->message;
            emit get_messages(resp->message);
        }else if (resp->command == "close_connections"){
            emit closeConnection();
        }
        else
           qDebug() << "Не известная команда: " << resp->command;
    }
}

appSettings *QmlWebSocket::get_settings()
{
    return settings;
}

const QString QmlWebSocket::getUserName()
{
    if(settings)
        return settings->RootUser;
    else
        return "";
}

void QmlWebSocket::setUserName(const QString &name)
{
    if(settings)
        settings->RootUser = name;

    user = name;
}

const QString QmlWebSocket::getHash()
{
    if(settings){
        return settings->Hash;
    }else
        return "";
}

bool QmlWebSocket::isStarted()
{
    if(client){
        return client->started();
    }else
        return false;
}

const QString QmlWebSocket::getUserUUID()
{
    return QString::fromStdString(client->get_user_uuid());
}

const QString QmlWebSocket::getActivePage()
{
    return activePage;
}

void QmlWebSocket::setActivePage(const QString& page)
{
    activePage = page;
}

void QmlWebSocket::getMessages(const QString &uuid_sub, int start_date, int end_date, int limit, const QString &uuid_form)
{
    if(client->started())
        client->get_messages(uuid_sub.toStdString(), start_date, end_date, limit, uuid_form.toStdString());
}

long QmlWebSocket::currentDate()
{
    return arc_json::current_date_seconds();
}

long QmlWebSocket::addDay(const long source, const int dayCount)
{
    return arc_json::add_day(source, dayCount);
}
