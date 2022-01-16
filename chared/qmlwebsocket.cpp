#include "qmlwebsocket.h"
#include <QJsonObject>
#include <QJsonDocument>

#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QBuffer>
#include <QMimeDatabase>


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

    //pWebDav= new bWebDav(this, settings.confFileName());
    qWebdav = new QWebdav(this, settings.confFileName());
    connect(qWebdav, &QWebdav::errorChanged, this, &bWebSocket::onWebDavError);
    connect(qWebdav, &QWebdav::downloadFinished, this, &bWebSocket::downloadFinished);
    connect(qWebdav, &QWebdav::uploadFinished, this, &bWebSocket::uploadFinished);
    connect(qWebdav, &QWebdav::downloadError, this, &bWebSocket::downloadError);
    connect(qWebdav, &QWebdav::uploadError, this, &bWebSocket::uploadError);
    connect(qWebdav, &QWebdav::downloadProgress, this, &bWebSocket::onDownloadProgress);
    connect(qWebdav, &QWebdav::uploadProgress, this, &bWebSocket::onUploadProgress);

    m_is_agent = false;
    m_hidden = false;
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
    //qDebug() << "bWebSocket::saveCache: " << qPrintable(jsonText);
    client->set_user_cache(jsonText.toStdString(), "");
}

void bWebSocket::messages(const QString &uuid)
{
    long int current_date = currentDate();
    long int start_date = addDay(current_date, -10);

    get_messages(uuid, start_date, current_date);
}

void bWebSocket::sendMessage(const QString &recipient, const QString &msg, const QString& objectName, const QString& msg_ref)
{
    if(client->started()){
        //сдесь в base64 и полностью сообщение из-за html конфликтов при чтении json
        std::string _message = msg.toUtf8().toBase64().toStdString();
        client->send(_message, recipient.toStdString(), "", objectName.toStdString(), msg_ref.toStdString());
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
        std::string param = QString("{\"uuid_user\":\"%1\", \"app_name\": \"qt_client\"}").arg(uuid).toStdString();
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
#ifdef QT_QML_CLIENT_APP
            client->send_command("get_webdav_settings", "", "");
#endif
        }
        else if(resp->command == "get_unread_messages"){
            emit unreadMessages(resp->message);
        }
        else if(resp->command == "command_to_qt_client"){
#ifdef QT_QML_CLIENT_APP
            qDebug() << resp->message;
            if(resp->message == "clientShow"){
                emit clientShow();
            }
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
        }else if (resp->command == "command_to_qt_agent"){
#ifdef QT_AGENT_APP
            responseCommand(resp);
#endif
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
    if(client->started())
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
    if(client->started())
        client->remove_user(uuid.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::setParent(const QString &uuid, const QString &parent)
{
    if(client->started())
        client->set_parent(uuid.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::killSession(const QString &uuid)
{
    if(client->started())
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
    if(client->started())
        client->set_webdav_settings_on_server();
}

bool bWebSocket::isAgentUse()
{
    return !uuidSessionAgent.isEmpty();
}


bool bWebSocket::isAgent()
{
    return m_is_agent;
}

void bWebSocket::setIsAgent(bool val)
{
    m_is_agent = val;
}


void bWebSocket::setUuidSessAgent(const QString &uuid)
{
    qDebug() << "bWebSocket::setUuidSessAgent::registerClient";
    uuidSessionAgent = uuid;
    QJsonObject obj = QJsonObject();
    obj.insert("uuid_agent", uuidSessionAgent);
    obj.insert("uuid_client", getUuidSession());
    obj.insert("command", "registerClient");
    obj.insert("message", "{\"command\":\"registerClient\", \"message\":\"Регистрация клиента у агента.\"}");

    QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    if(client->started()){
         client->send_command("command_to_qt_agent", "", param.toStdString());
    }

}

void bWebSocket::getUsers(const QString &parent)
{
    if(client->started())
        client->get_users(parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::addGroup(const QString &name, const QString &presentation, const QString &parent)
{
    if(client->started())
        client->add_group(name.toStdString(), presentation.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::editGroup(const QString &uuid, const QString &name, const QString &presentation, const QString &parent)
{
    if(client->started())
        client->edit_group(uuid.toStdString(), name.toStdString(), presentation.toStdString(), parent.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::removeGroup(const QString &uuid)
{
    if(client->started())
        client->remove_group(uuid.toStdString(), IClient::nil_string_uuid());
}

void bWebSocket::sendCommand(const QString &cmd, const QString &uuidForm, const QString &param)
{
    std::string form = uuidForm.toStdString();
    if(form.empty())
        form = IClient::nil_string_uuid();

    if(client->started())
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

QStringList bWebSocket::getImageMimeType()
{
    QStringList result = {
        "image/png",
        "image/jpeg",
        "image/gif",
        "image/bmp",
        "image/vnd.microsoft.icon",
        "image/tiff",
        "image/svg+xml"
    };

    return result;
}

void bWebSocket::responseCommand(ServeResponse * resp)
{
    QJsonDocument doc = QJsonDocument::fromJson(resp->message.toUtf8());
    if(!doc.isObject())
        return;
    QJsonObject obj = doc.object();

    QString command = obj.value("command").toString();
    QString message = obj.value("message").toString();
    if(command == "registerClient")
        joinClientToAgent(resp);
    else if(command== "displayError")
        emit displayError("Ошибка", message);
    else if(command == "hiddenChanged"){
        if (message == "true")
            m_hidden = true;
        else
            m_hidden = false;
        if(m_is_agent){
            emit hiddenChanged(m_hidden);
        }
    }
}

void bWebSocket::registerToAgent(const QString &uuid) {
    qDebug() << "bWebSocket::registerToAgent: " << uuid;
}

//void bWebSocket::registerClientForAgent(const QString &uuid) {

////    QJsonObject param = QJsonObject();
////    param.insert("uuid_client", uuid);
////    param.insert("command", "setClient");

////    QString _param = QJsonDocument(param).toJson(QJsonDocument::Indented);

////    if(client->started()){
////        client->send_command("command_to_qt_agent", "", _param.toStdString());
////    }

//}

void bWebSocket::uploadFile(const QString &token, const QString &fileName, const QString &ref)
{
    qDebug() << "bWebSocket::uploadFile: " << token << " " << fileName;

    if(token.isEmpty())
    {
        qDebug() << "bWebSocket::uploadFile: не указан токен чата!";
        return;
    }

    if(!qWebdav->exists(token)){
        qDebug() << "bWebSocket::uploadFile::createDirectory: " <<  token;
        bool result = qWebdav->mkdirSynch(token);
        if(!result)
        {
            qDebug() << "error create chat cache folder!";
            return;
        }
    }

    qWebdav->uploadFile(token, fileName, ref);

}

void
bWebSocket::downloadFile(const QString &token, const QString &fileName, const QString &ref, const QString &outputDir)
{
    qDebug() << "downloadFile: " << token << " " << fileName << " " << ref << " " << outputDir;
    if(token.isEmpty())
    {
        qDebug() << "bWebSocket::uploadFile: не указан токен чата!";
        return;
    }

    qWebdav->downloadFile(token, fileName, ref, outputDir);

}

bool bWebSocket::verifyLocalRoomCacheDirectory(const QString& roomToken)
{
    return ClientSettings::verifyLocalRoomCacheDirectory(roomToken);
}

bool bWebSocket::saveFileToUserCache(const QString &token, const QString &localFile, const QString& refMessage)
{
    QFile file(localFile);
    if(!file.exists())
        return false;
    if(ClientSettings::verifyLocalRoomCacheDirectory(token)){
          QString newFile = ClientSettings::getUserRoomsCacheDirectory() + QDir::separator() + token
                  + file.fileName() + "." + refMessage;
          QFile nFile(newFile);
          if(!nFile.exists()){
              file.copy(newFile);
              return true;
          }
    }

    return false;

}

QString bWebSocket::getFileName(const QString &filePath)
{
    QString _fp = filePath;

    if(_fp.startsWith("file:///"))
        _fp = QUrl(filePath).toLocalFile();

    if(!_fp.isEmpty()){
        _fp = QDir::toNativeSeparators(_fp);
    }

    QFile file(_fp);

    QString name = QFileInfo(_fp).fileName();

    return name;
}

QString bWebSocket::getObjectHtmlSource(const QString &fileName)
{
    if(fileName.isEmpty())
        return {};

    QString _fp = fileName;

    if(_fp.startsWith("file:///"))
        _fp = QUrl(fileName).toLocalFile();

    if(!_fp.isEmpty()){
        _fp = QDir::toNativeSeparators(_fp);
    }

    QFile file(_fp);

    QString name = QFileInfo(_fp).fileName();

    if (!file.open(QIODevice::ReadOnly))
        return {};

    QMimeDatabase db;
    QMimeType _mime = db.mimeTypeForFile(_fp, QMimeDatabase::MatchContent);
    QString format = _mime.preferredSuffix();
    QString mime = _mime.name();
    QStringList suff = getImageMimeType();

    bool is_image = false;
    if(!mime.isEmpty())
        is_image = suff.indexOf(mime) != -1;

    if(!is_image){
        return "<a href=\"" + fileName + "\">"+name+"</a>";
    }
    else{

        QByteArray _data = file.readAll();
        QString base64;

        auto *img = new QImage();
        img->loadFromData(_data);
        if(img->size().height() > 400 || img->size().width() > 400){
            QSize size = img->size();
            if(size.height() > 400){
                float p = 400 / (float)size.height();
                size.setHeight((int)(p * (float)size.height()));
                size.setWidth((int)(p * (float)size.width()));
            }
            if(size.width() > 400){
                float p = 400 / (float)size.width();
                size.setHeight((int)(p * (float)size.height()));
                size.setWidth((int)(p * (float)size.width()));
            }
            QImage small = img->scaled(size);
            QByteArray arr;
            QBuffer buffer(&arr);
            buffer.open(QIODevice::WriteOnly);
            small.save(&buffer, format.toUpper().toLocal8Bit().data());
            base64 = arr.toBase64();
        }else
            base64 = _data.toBase64();

        return "<img src=\"data:" + mime + ";base64," + base64 + "\"><br><a href=\"" + fileName + "\">" + name + "</a>";
    }

}

void bWebSocket::setAppName(const QString &name) {
    settings[bConfFieldsWrapper::AppName] = name;
    settings.save();
}

bool bWebSocket::isImage(const QString& fileName) {

    QString _fp = fileName;
    if(_fp.startsWith("file:///"))
        _fp = QUrl(fileName).toLocalFile();
    if(_fp.isEmpty()){
        return false;
    }

    QMimeDatabase db;
    QMimeType _mime = db.mimeTypeForFile(_fp, QMimeDatabase::MatchContent);
    if(_mime.name().isEmpty())
        return false;
    QStringList suff = getImageMimeType();
    return suff.indexOf(_mime.name()) != -1;

}

bool bWebSocket::fileExists(const QString &path)
{
    QString _fp = path;
    if(_fp.startsWith("file:///"))
        _fp = QUrl(path).toLocalFile();
    if(_fp.isEmpty()){
        return false;
    }

    QFile file(_fp);
    return file.exists();
}

QString bWebSocket::getRandomUUID()
{
    return QString::fromStdString(client->get_string_random_uuid());
}

QString bWebSocket::separator()
{
    return QDir::separator();
}

void bWebSocket::onWebDavError(QNetworkReply::NetworkError type, const QString &error) {
    qDebug() << type << ":" << error;
    emit webDavError(error);
}

void bWebSocket::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesTotal == 0){
        emit setProgress(0);
        return;
    }

    double result = (double)bytesReceived / (double)bytesTotal;
    emit setProgress(result);
    //qDebug() << "bWebSocket::onDownloadProgress:" << result;
}

void bWebSocket::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if(bytesTotal == 0){
        emit setProgress(0);
        return;
    }

    double result = (double)bytesSent / (double)bytesTotal;
    emit setProgress(result);
    //qDebug() << "bWebSocket::onDownloadProgress:" << result;
}

void bWebSocket::downloadFinished()
{
    //qDebug() << "успешная загрузка файла с сервера!";
    //emit webDavError("успешная загрузка файла с сервера!");
}

void bWebSocket::uploadFinished()
{
    //qDebug() << "успешная загрузка файла на сервер!";
    //emit webDavError("успешная загрузка файла на сервер!");
}

void bWebSocket::downloadError()
{
    qDebug() << "bWebSocket::downloadError";
    emit webDavError("Ошибка загрузки файла с сервера!");
    if(isAgentUse()){
        QJsonObject obj = QJsonObject();
        obj.insert("uuid_agent", uuidSessionAgent);
        obj.insert("uuid_client", getUuidSession());
        obj.insert("command", "displayError");
        obj.insert("message", "{\"command\":\"displayError\", \"message\":\"Ошибка загрузки файла с сервера!\"}");


        QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
        if(client->started()){
             client->send_command("command_to_qt_agent", "", param.toStdString());
        }

    }
}

void bWebSocket::sendCommandToAgent(const QString &cmd, const QString &msg) {

    qDebug() << "bWebSocket::sendCommandToAgent";

    if(isAgentUse()){
        QJsonObject obj = QJsonObject();
        obj.insert("uuid_agent", uuidSessionAgent);
        obj.insert("uuid_client", getUuidSession());
        obj.insert("command", cmd);
        obj.insert("message", QString("{\"command\":\"%1\", \"message\":\"%2\"}").arg(cmd, msg));


        QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
        if(client->started()){
            client->send_command("command_to_qt_agent", "", param.toStdString());
        }

    }
}

void bWebSocket::uploadError()
{
    //qDebug() << "ошибка загрузки файла на сервера!";
    emit webDavError("ошибка загрузки файла на сервер!");
}

std::string bWebSocket::get_std_parent_path() {
    return IClient::get_parent_path();
}

void bWebSocket::setHidden(bool val) {
    m_hidden = val;
    emit hiddenChanged(m_hidden);
}

bool bWebSocket::isHidden() {
    return m_hidden;
}
