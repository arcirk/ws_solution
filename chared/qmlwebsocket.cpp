#include "qmlwebsocket.h"
#include <QJsonObject>
#include <QJsonDocument>

#include <QFileInfo>
#include <QDir>

#ifdef QT_QML_CLIENT_APP
#include "../../../ws_client_qml/ws_gclient/include/webdav.h"
#endif

bWebSocket::bWebSocket(QObject *parent, const QString& confFile)
: QObject(parent),
  settings(confFile, false, true)
{
    //Предполагаем что если не указан конфиг файл тогда инициализация происходит из qml модуля
    //по этому запрещаем сохранять настройки при инициализации - no_save_def = true
    //если имя файла не указано то настройки загрузятся по умолчанию в файле conf.json, нам это не нужно
    //по этому инициализируем снова
    if (confFile.isEmpty()){
        //разрешим сохранение с указанным именем файла, или загрузим сохраненные настройки
        settings = ClientSettings("conf_qt_client.json", false);
    }


    _callback_message callback = std::bind(&bWebSocket::ext_message, this, std::placeholders::_1);
    _callback_status callback_status = std::bind(&bWebSocket::status_changed, this, std::placeholders::_1);

    client = new IClient(settings[bConfFieldsWrapper::ServerHost].toString().toStdString(), settings[bConfFieldsWrapper::ServerPort].toInt(), callback, callback_status);

    _pwdEdit = false;

}

bWebSocket::~bWebSocket()
{
    if (client->started()) {
        client->close();
    }

    m_agentClients.clear();

    delete client;

}

void bWebSocket::open(const QString& user_name, const QString& user_password)
{
    if (client->started()) {
        return;
    }

    QString _hash;

    if(_pwdEdit){
        settings[bConfFieldsWrapper::Hash] = QString::fromStdString( IClient::get_hash(user_name.toStdString(), user_password.toStdString()));
    }
    settings[bConfFieldsWrapper::User] = user_name;

    client->admin_name = settings[bConfFieldsWrapper::User].toString().toStdString();
    client->hash = settings[bConfFieldsWrapper::Hash].toString().toStdString();
    client->host = settings[bConfFieldsWrapper::ServerHost].toString().toStdString();
    client->port = settings[bConfFieldsWrapper::ServerPort].toInt();
    client->app_name = settings[bConfFieldsWrapper::AppName].toString().toStdString();

    client->open();

    settings.save();

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
#ifdef QT_QML_CLIENT_APP
            client->get_users_catalog("");
#else
            client->send_command("get_webdav_settings", "", "");
#endif
        }else if (resp->command == "get_users_catalog"){
#ifdef QT_QML_CLIENT_APP
            emit resetUsersCatalog(resp->message);
            client->get_user_cache("");
#endif
        }else if (resp->command == "get_user_cache"){
#ifdef QT_QML_CLIENT_APP
            std::string base64 =  resp->message.toStdString();
            QString msg = QString::fromStdString(IClient::base64_decode(base64));
            emit getUserCache(msg);
            client->send_command("get_active_users", IClient::nil_string_uuid(), "{\"app_name\": \"qt_client\", \"unique\": \"true\"}");
#endif
        }else if(resp->command == "get_active_users"){
            emit getActiveUsers(resp->message);
#ifdef QT_QML_CLIENT_APP
            client->send_command("get_unread_messages", "", "");
#endif
        }else if (resp->command == "get_messages"){
#ifdef QT_QML_CLIENT_APP
            client->reset_unread_messages(resp->recipient.toStdString(), "");
#endif
            emit setMessages(resp->message);
        }else if (resp->command == "close_connections"){
            emit closeConnection();            
        }else if (resp->command == "message"){
            emit messageReceived(resp->message, resp->uuid, resp->recipient, resp->recipientName);
        }else if (resp->command == "set_user_cache"){
#ifdef QT_QML_CLIENT_APP
            std::string base64 =  resp->message.toStdString();
            QString msg = QString::fromStdString(IClient::base64_decode(base64));
            emit getUserCache(msg);
            client->send_command("get_active_users", IClient::nil_string_uuid(), "{\"app_name\": \"qt_client\", \"unique\": \"true\"}");
#endif
        }else if (resp->command == "get_user_info"){
            emit userInfo(resp->message);
        }else if (resp->command == "client_join"){
            emit clientJoin(resp->message);
        }else if(resp->command == "client_leave"){
            emit clientLeave(resp->message);
        }else if(resp->command == "get_user_status"){
            emit requestUserStatus(resp->message);
        }else if(resp->command == "reset_unread_messages"){
#ifdef QT_QML_CLIENT_APP
            client->send_command("get_webdav_settings", "", "");
#endif
        }
        else if(resp->command == "get_unread_messages"){
            emit unreadMessages(resp->message);
        }
        else if(resp->command == "command_to_qt_client"){
#ifdef QT_QML_CLIENT_APP
            if(resp->message == "clientShow")
                emit clientShow();
#endif
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
            emit addUser(resp->message);
        }else if (resp->command == "remove_user"){
            emit deleteUser(resp->message);
        }else if (resp->command == "update_user"){
            emit updateUser(resp->message);
        }else if (resp->command == "set_parent"){
            emit setUserParent(resp->message);
        }else if(resp->command == "get_channel_token"){
#ifdef QT_QML_CLIENT_APP
            qDebug() << "get_channel_token: " << resp->message;
            //client->reset_unread_messages(resp->recipient.toStdString(), "");
            emit getChannelToken(resp->message);
#endif
        }else if (resp->command == "get_webdav_settings"){
            setWebDavSettingsToClient(resp->message);
        }else if (resp->command == "set_webdav_settings"){
            //обновились настройки webdav на сервере
        }
        else
           qDebug() << "Не известная команда: " << resp->command;
    }

    delete resp;
}


QString bWebSocket::getUserName() const
{
//    if(settings)
        return settings[bConfFieldsWrapper::User].toString();
//    else
//        return "";
}

void bWebSocket::setUserName(const QString &name)
{
    settings[bConfFieldsWrapper::User] = name;
    settings.save();
    user = name;
}

QString bWebSocket::getHash() const
{
    return settings[bConfFieldsWrapper::Hash].toString();
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
    settings[bConfFieldsWrapper::ServerHost] = newHost;
    settings.save();
}

QString bWebSocket::getHost()
{
    return settings[bConfFieldsWrapper::ServerHost].toString();
}

void bWebSocket::setPort(int newPort)
{
    settings[bConfFieldsWrapper::ServerPort]= newPort;
    settings.save();
}

int bWebSocket::getPort()
{
    return settings[bConfFieldsWrapper::ServerPort].toInt();
}

bool bWebSocket::autoConnect()
{
    return settings[bConfFieldsWrapper::AutoConnect].toBool();
}

void bWebSocket::setAutoConnect(bool value)
{
    settings[bConfFieldsWrapper::AutoConnect] = value;
    settings.save();
}

bool bWebSocket::saveHash()
{
    return settings[bConfFieldsWrapper::SaveHash].toBool();
}

void bWebSocket::setSaveHash(bool value)
{
    settings[bConfFieldsWrapper::SaveHash] = value;
    settings.save();
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
    confFileName = fname;
    settings = ClientSettings(fname, false);
}

QString bWebSocket::getSettingsFileName()
{
    return confFileName;
}

void bWebSocket::updateSettings()
{
    QString confFileName = settings.confFileName();
    settings = ClientSettings(confFileName, false);
    client->set_webdav_settings_on_client(settings.to_string().toStdString());
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

void bWebSocket::setWebDavSettingsToClient(const QString &resp)
{
    client->set_webdav_settings_on_client(resp.toStdString());

    settings[bConfFieldsWrapper::WebDavHost] = QString::fromStdString(client->get_webdav_host());
    settings[bConfFieldsWrapper::WebDavUser] = QString::fromStdString(client->get_webdav_user());
    settings[bConfFieldsWrapper::WebDavPwd] = QString::fromStdString(client->get_webdav_pwd());
    settings[bConfFieldsWrapper::WebDavSSL] = client->get_webdav_ssl();

    settings.save();
}

void bWebSocket::setWebDavSettingsToServer()
{
    updateSettings();
    client->set_webdav_settings_on_server();
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

ClientSettings &bWebSocket::options() {
    return settings;
}

void bWebSocket::agentClientShow() {

    for (auto itr = m_agentClients.begin(); itr != m_agentClients.end(); ++itr) {
        if (itr.value() == "qt_client"){

            QJsonObject obj = QJsonObject();
            obj.insert("uuid_agent", getUuidSession());
            obj.insert("uuid_client", itr.key());
            obj.insert("command", "clientShow");


            QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
            //qDebug() << param;

            if(client->started()){
                client->send_command("command_to_qt_client", "", param.toStdString());
            }
        }
    }

}


void bWebSocket::joinClientToAgent(ServeResponse *resp) {

    if(resp->uuid_session.isEmpty())
        return;

    auto itr = m_agentClients.find(resp->uuid_session);
    if(itr != m_agentClients.end())
        return;

    m_agentClients.insert(resp->uuid_session, resp->app_name);

}

void bWebSocket::registerToAgent(const QString &uuid) {


}

void bWebSocket::registerClientForAgent(const QString &uuid) {

    QJsonObject param = QJsonObject();
    param.insert("uuid_client", uuid);
    param.insert("command", "setClient");
    QString _param = QJsonDocument(param).toJson(QJsonDocument::Indented);

    if(client->started()){
        client->send_command("command_to_qt_agent", "", _param.toStdString());
    }

}

void bWebSocket::uploadFile(const QString &token, const QString &fileName)
{
    qDebug() << "uploadFile: " << token << " " << fileName;


#ifdef QT_QML_CLIENT_APP
    QWebDav webDav = QWebDav(this);
    webDav.setHost(settings[bConfFieldsWrapper::WebDavHost].toString());
    webDav.setUser(settings[bConfFieldsWrapper::WebDavUser].toString());
    webDav.setPassword(settings[bConfFieldsWrapper::WebDavPwd].toString());
    webDav.setSsl(settings[bConfFieldsWrapper::WebDavSSL].toBool());
    webDav.uploadFile(token,fileName);
#endif

}

void bWebSocket::downloadFile(const QString &token, const QString &fileName)
{
    qDebug() << "downloadFile: " << token << " " << fileName;
}

void bWebSocket::setAppName(const QString &name) {
    settings[bConfFieldsWrapper::AppName] = name;
    settings.save();
}
