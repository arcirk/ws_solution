#include "../include/qmlwebsocket.h"

bWebSocket::bWebSocket(QObject *parent) : QObject(parent)
{

    settings = new ClientSettings();
    settings->init();

    _callback_message callback = std::bind(&bWebSocket::ext_message, this, std::placeholders::_1);
    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback);
}

bWebSocket::~bWebSocket()
{
    if (client->started()) {
        client->close();
    }

    delete client;

}

void bWebSocket::open(const QString& user_name, const QString& user_password)
{
    if (client->started()) {
        return;
    }

    if (!user_password.isEmpty()) {
        settings->Hash = QString::fromStdString( get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    if (settings->Hash.isEmpty()) {
        settings->Hash = QString::fromStdString( get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    settings->RootUser = user_name;

    client->admin_name = settings->RootUser.toStdString();
    client->hash = settings->Hash.toStdString();
    client->host = settings->ServerHost.toStdString();
    client->port = settings->ServerPort;

    client->open();

    settings->save_settings();

    emit startedChanged();
}

void bWebSocket::close()
{
    if(client->started()){
        client->close();
    }

    emit startedChanged();
}

void bWebSocket::ext_message(const std::string &msg)
{
    QString resp = ServeResponse::base64_decode(msg);
    //qDebug() << QString::fromStdString(msg);

    if(!resp.isEmpty()){
        processServeResponse(resp);
    }

}

void bWebSocket::processServeResponse(const QString &jsonResp)
{
    //qDebug() << jsonResp;
    auto resp = new ServeResponse(jsonResp);

    if(!resp->isParse){
        delete resp;
        return;
    }
    if(resp->result == "error"){

            emit displayError(resp->command, resp->message);
            emit qmlError(resp->command, resp->message);

            if(resp->command == "set_client_param")
                client->close();

    }else
    {
        if(resp->command == "set_client_param"){
            emit displayNotify("Подключился к серверу.");
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

ClientSettings *bWebSocket::get_settings()
{
    return settings;
}

const QString bWebSocket::getUserName()
{
    if(settings)
        return settings->RootUser;
    else
        return "";
}

void bWebSocket::setUserName(const QString &name)
{
    if(settings)
        settings->RootUser = name;

    user = name;
}

const QString bWebSocket::getHash()
{
    if(settings){
        return settings->Hash;
    }else
        return "";
}

bool bWebSocket::isStarted()
{
    if(client){
        return client->started();
    }else
        return false;
}

const QString bWebSocket::getUserUUID()
{
    return QString::fromStdString(client->get_user_uuid());
}

//const QString bWebSocket::getActivePage()
//{
//    return activePage;
//}
//
//void bWebSocket::setActivePage(const QString& page)
//{
//    activePage = page;
//}

void bWebSocket::getMessages(const QString &uuid_sub, int start_date, int end_date, int limit, const QString &uuid_form)
{
    if(client->started())
        client->get_messages(uuid_sub.toStdString(), start_date, end_date, limit, uuid_form.toStdString());
}

long bWebSocket::currentDate()
{
    return current_date_seconds();
}

long bWebSocket::addDay(const long source, const int dayCount)
{
    return add_day(source, dayCount);
}

void bWebSocket::setHost(const QString &newHost)
{
    settings->ServerHost = newHost;
}

const QString &bWebSocket::getHost()
{
    return settings->ServerHost;
}

void bWebSocket::setPort(int newPort)
{
    settings->ServerPort = newPort;
}

int bWebSocket::getPort()
{
    return settings->ServerPort;
}

bool bWebSocket::getStarted()
{
    if(client)
        return client->started();
    else
        return false;
}
