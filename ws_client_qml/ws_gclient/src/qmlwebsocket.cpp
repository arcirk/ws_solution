#include "../include/qmlwebsocket.h"

bWebSocket::bWebSocket(QObject *parent) : QObject(parent)
{

    settings = new ClientSettings();
    settings->init();

    _callback_message callback = std::bind(&bWebSocket::ext_message, this, std::placeholders::_1);
    _callback_status callback_status = std::bind(&bWebSocket::status_changed, this, std::placeholders::_1);

    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback, callback_status);
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

    QString _hash;

    if(_pwdEdit){
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

void bWebSocket::saveCache(const QString &jsonText)
{
    client->set_user_cache(jsonText.toStdString(), "");
}

void bWebSocket::messages(const QString &uuid)
{
    long int current_date = currentDate();
    long int start_date = addDay(current_date, -10);

    get_messages(uuid, start_date, current_date);
}

void bWebSocket::sendMessage(const QString &recipient, const QString &msg)
{
    if(client->started()){
        //сдесь в base64 и полностью сообщение из-за html конфликтов при чтении json
        std::string _message = msg.toUtf8().toBase64().toStdString();
        client->send(_message, recipient.toStdString(), "");
    }
}

void bWebSocket::ext_message(const std::string &msg)
{
    QString resp = ServeResponse::base64_decode(msg);
    //qDebug() << QString::fromStdString(msg);

    if(!resp.isEmpty()){
        processServeResponse(resp);
    }

}

void bWebSocket::status_changed(bool status)
{
    emit connectedStatusChanged(status);
}

void bWebSocket::processServeResponse(const QString &jsonResp)
{
    //qDebug() << jsonResp;
    //QString _tmp = jsonResp;
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
            client->send_command("set_content_type", "", "{\"content_type\":\"HTML\"}");
        }else if (resp->command == "set_content_type"){
            client->send_command("set_message_struct_type", "", "{\"struct_type\":\"DB\"}");
        }else if (resp->command == "set_message_struct_type"){
            client->get_users_catalog("");
        }else if (resp->command == "get_users_catalog"){
            emit resetUsersCatalog(resp->message);
            client->get_user_cache("");
        }else if (resp->command == "get_user_cache"){
            std::string base64 =  resp->message.toStdString();
            QString msg = QString::fromStdString(arcirk::base64_decode(base64));
            emit getUserCache(msg);
        }else if (resp->command == "get_messages"){
            //qDebug() << resp->message;
            emit setMessages(resp->message);
        }else if (resp->command == "close_connections"){
            emit closeConnection();            
        }else if (resp->command == "message"){
            emit messageReceived(resp->message, resp->uuid, resp->recipient);
        }else if (resp->command == "set_user_cache"){
            //
        }
        else
           qDebug() << "Не известная команда: " << resp->command;
    }

    delete resp;
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

const QString bWebSocket::getUuidSession()
{
    return QString::fromStdString(client->get_app_uuid());
}


void bWebSocket::get_messages(const QString &uuid_sub, int start_date, int end_date, int limit, const QString &uuid_form)
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

bool bWebSocket::autoConnect()
{
    return settings->AutoConnect;
}

void bWebSocket::setAutoConnect(bool value)
{
    settings->AutoConnect = value;
}

bool bWebSocket::saveHash()
{
    return settings->SaveHash;
}

void bWebSocket::setSaveHash(bool value)
{
    settings->SaveHash = value;
}

bool bWebSocket::pwdEdit()
{
    return _pwdEdit;
}

void bWebSocket::setPwdEdit(bool value)
{
    _pwdEdit = value;
}

bool bWebSocket::connectedStatus()
{
    if(client){
        return client->is_open();
    }else
        return false;
}

