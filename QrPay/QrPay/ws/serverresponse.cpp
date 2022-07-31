#include "serverresponse.h"

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>

ServerResponse::ServerResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

QString ServerResponse::base64_decode(const QByteArray &resp)
{

    return QByteArray::fromBase64(resp);
}

QString ServerResponse::base64_encode(const QByteArray &resp)
{
    return resp.toBase64();
}

void ServerResponse::parse(const QString& resp){


    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    if(!_doc.isEmpty())
    {
        if(_doc.isObject())
        {
            QJsonObject obj = _doc.object();
            auto _message = obj.find("message");
            auto _result = obj.find("result");
            auto _command = obj.find("command");
            auto _recipient = obj.find("uuid_channel");
            auto _uuid = obj.find("user_uuid");
            auto _uuid_session= obj.find("uuid");
            auto _contentType = obj.find("contentType");
            auto _recipientName = obj.find("channel_name");
            auto _app_name = obj.find("app_name");
            auto _ip_address = obj.find("ip_address");
            auto _host_name = obj.find("host_name");
            auto _user_name = obj.find("user_name");

             if(_message->isString()){
                 message = _message.value().toString();
             }
             if(_command->isString()){
                 command = _command.value().toString();
             }
             if(_result->isString()){
                 result = _result.value().toString();
             }
             if(_recipient->isString()){
                 recipient = _recipient.value().toString();
             }
             if(_uuid->isString()){
                 uuid = _uuid.value().toString();
             }
             if(_contentType->isString()){
                 contentType = _contentType.value().toString();
             }
             if(_contentType->isString()){
                 recipientName = _recipientName.value().toString();
             }
             if(_app_name->isString()){
                 app_name = _app_name.value().toString();
             }
             if(_uuid_session->isString()){
                 uuid_session = _uuid_session.value().toString();
             }
             if(_ip_address->isString()){
                 ip_address = _ip_address.value().toString();
             }
             if(_host_name->isString()){
                 host_name = _host_name.value().toString();
             }
             if(_user_name->isString()){
                 user_name = _user_name.value().toString();
             }
            isParse = true;
        }
    }

}


QString ServerResponse::to_string() const
{
    QJsonObject msg = QJsonObject();
    msg.insert("message", message);
    msg.insert("command", command);
    msg.insert("result", result);
    msg.insert("recipient", recipient);
    msg.insert("token", token);
    msg.insert("uuid", uuid);
    msg.insert("contentType", contentType);
    msg.insert("recipientName", recipientName);
    msg.insert("app_name", app_name);
    msg.insert("uuid_session", uuid_session);
    msg.insert("ip_address", ip_address);
    msg.insert("host_name", host_name);
    msg.insert("user_name", user_name);

    return QJsonDocument(msg).toJson(QJsonDocument::Indented);
}

