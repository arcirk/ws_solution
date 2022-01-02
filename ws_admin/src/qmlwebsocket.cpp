#include "../include/qmlwebsocket.h"
#include "../../../ws_agent/ws_agent/include/qmlwebsocket.h"


#include <QJsonObject>
#include <QJsonDocument>

#include <QFileInfo>
#include <QDir>

bWebSocket::bWebSocket(QObject *parent) : QObject(parent)
{

    settings = new ClientSettings();
    settings->init();

    _callback_message callback = std::bind(&bWebSocket::ext_message, this, std::placeholders::_1);
    _callback_status callback_status = std::bind(&bWebSocket::status_changed, this, std::placeholders::_1);

    client = new IClient(settings->ServerHost.toStdString(), settings->ServerPort, callback, callback_status);

    _pwdEdit = false;

   // emit initDone();
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
        settings->Hash = QString::fromStdString( IClient::get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    settings->RootUser = user_name;

    client->admin_name = settings->RootUser.toStdString();
    client->hash = settings->Hash.toStdString();
    client->host = settings->ServerHost.toStdString();
    client->port = settings->ServerPort;
    client->app_name = settings->AppName.toStdString();

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

void bWebSocket::getUserInfo(const QString &uuid)
{
    if(client->started()){
        client->get_user_info(uuid.toStdString(), "");
    }
}

void bWebSocket::getUserStatus(const QString &uuid)
{
    if(client->started()){
        std::string param = QString("{\"uuid_user\":\"%1\", \"app_name\": \"qt_client\"}").arg(uuid).toStdString();//IClient::str_sample("{\"uuid_user\":\"%1%\", \"app_name\": \"qt_client\"}", uuid.toStdString());
        client->get_user_status(uuid.toStdString(), "", param);
    }
}

void bWebSocket::getUserData(const QString &uuid, const QString &param)
{
    if(client->started()){
        client->get_user_data(uuid.toStdString(), "", param.toStdString());
    }
}

void bWebSocket::resetUnreadMsgFromData(const QString &sender)
{
    if(client->started()){
        client->reset_unread_messages(sender.toStdString(), "");
    }
}

void bWebSocket::registerToAgent()
{
    if(client->started()){
        //client->se
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
            //client->get_users_catalog("");
        }else if (resp->command == "get_users_catalog"){
            //emit resetUsersCatalog(resp->message);
            //client->get_user_cache("");
        }else if (resp->command == "get_user_cache"){
            //std::string base64 =  resp->message.toStdString();
            //QString msg = QString::fromStdString(IClient::base64_decode(base64));
            //emit getUserCache(msg);
            //client->send_command("get_active_users", IClient::nil_string_uuid(), "{\"app_name\": \"qt_client\", \"unique\": \"true\"}");
        }else if(resp->command == "get_active_users"){
            emit getActiveUsers(resp->message);
            //client->send_command("get_unread_messages", "", "");
        }else if (resp->command == "get_messages"){
            //client->reset_unread_messages(resp->recipient.toStdString(), "");
            emit setMessages(resp->message);
        }else if (resp->command == "close_connections"){
            emit closeConnection();            
        }else if (resp->command == "message"){
            emit messageReceived(resp->message, resp->uuid, resp->recipient, resp->recipientName);
        }else if (resp->command == "set_user_cache"){
            //
        }else if (resp->command == "get_user_info"){
            emit userInfo(resp->message);
        }else if (resp->command == "client_join"){
            emit clientJoin(resp->message);
        }else if(resp->command == "client_leave"){
            emit clientLeave(resp->message);
        }else if(resp->command == "get_user_status"){
            emit requestUserStatus(resp->message);
        }else if(resp->command == "reset_unread_messages"){
            //
        }
        else if(resp->command == "get_unread_messages"){
            emit unreadMessages(resp->message);
        }
        else if(resp->command == "command_to_qt_client"){
            if(resp->message == "clientShow")
                emit clientShow();
        }
        else if(resp->command == "get_group_list"){
            emit getGroupList(resp->message);
        }
        else if(resp->command == "get_users"){
            emit getListUsers(resp->message);
        }
        else if(resp->command == "add_group"){
            emit addGroupUsers(resp->message);
        }
        else if(resp->command == "edit_group"){
            emit editGroupUsers(resp->message);
        }
        else if(resp->command == "remove_group"){
            emit removeGroupUsers(resp->message);
        }else if (resp->command == "add_user"){
            //on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
            emit addUser(resp->message);
        }else if (resp->command == "remove_user"){
            //on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
            emit deleteUser(resp->message);
        }else if (resp->command == "update_user"){
            //on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
            emit updateUser(resp->message);
        }else if (resp->command == "set_parent"){
            //on_treeChannels_currentItemChanged(ui->treeChannels->currentItem(), nullptr);
            emit setUserParent(resp->message);
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

const QString bWebSocket::getAppName()
{
    //return "qt_client";
    return "admin_console";
}


void bWebSocket::get_messages(const QString &uuid_sub, int start_date, int end_date, int limit, const QString &uuid_form)
{
    if(client->started())
        client->get_messages(uuid_sub.toStdString(), start_date, end_date, limit, uuid_form.toStdString());
}

long bWebSocket::currentDate()
{
    return IClient::current_date_seconds();
}

long bWebSocket::addDay(const long source, const int dayCount)
{
    return IClient::add_day(source, dayCount);
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
        return client->started();
    }else
        return false;
}

void bWebSocket::setSettingsFileName(const QString &fname)
{
    fileName = fname;
    settings->setSettingsFileName(fname);
}

QString bWebSocket::getSettingsFileName()
{
    return fileName;
}

void bWebSocket::updateSettings()
{
    settings->init();
}

const QString bWebSocket::nilUuid()
{
    return QString::fromStdString(IClient::nil_string_uuid());
}

const QString bWebSocket::randomUuid()
{
    return QString::fromStdString(IClient::random_uuid());
}

void bWebSocket::removeUser(const QString &uuid)
{
    client->remove_user(uuid.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::setParent(const QString &uuid, const QString &parent)
{
    client->set_parent(uuid.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::killSession(const QString &uuid)
{
    client->kill_session(uuid.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::setUuidSessAgent(const QString &uuid)
{
    uuidSessionAgent = uuid;
    QJsonObject obj = QJsonObject();
    obj.insert("uuid_agent", uuidSessionAgent);
    obj.insert("uuid_client", getUuidSession());
    obj.insert("command", "registerClient");


    QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    if(client->started()){
         client->send_command("command_to_qt_agent", "", param.toStdString());
    }

//    QString saveFileName = "testArgs.json";
//    QFileInfo fileInfo(saveFileName);
//    QDir::setCurrent(fileInfo.path());

//    QFile jsonFile(saveFileName);
//    if (!jsonFile.open(QIODevice::WriteOnly))
//    {
//        return;
//    }

//    // Записываем текущий объект Json в файл
//    jsonFile.write(param.toUtf8());
//    jsonFile.close();   // Закрываем файл

}

void bWebSocket::getUsers(const QString &parent)
{
    client->get_users(parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::addGroup(const QString &name, const QString &presentation, const QString &parent)
{
    client->add_group(name.toStdString(), presentation.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::editGroup(const QString &uuid, const QString &name, const QString &presentation, const QString &parent)
{
    client->edit_group(uuid.toStdString(), name.toStdString(), presentation.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::removeGroup(const QString &uuid)
{
    client->remove_group(uuid.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::sendCommand(const QString &cmd, const QString &uuidForm, const QString &param)
{
    std::string form = uuidForm.toStdString();
    if(form.empty())
        form = IClient::nil_string_uuid();

    client->send_command(cmd.toStdString(), form, param.toStdString());
}

