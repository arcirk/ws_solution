#include "serveresponse.h"

ServeResponse::ServeResponse(const QString& resp)
{

    isParse = false;
    parse(resp);
}

void ServeResponse::parse(const QString& resp){


    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    if(!_doc.isNull())
    {
        if(_doc.isObject())
        {
            QJsonObject obj = _doc.object();
            auto _message = obj.find("message");
            auto _result = obj.find("result");
            auto _command = obj.find("command");

             if(_message->isString()){
                 message = _message.value().toString();
             }
             if(_command->isString()){
                 command = _command.value().toString();
             }
             if(_result->isString()){
                 result = _result.value().toString();
             }

            isParse = true;
        }
    }

}

QJsonDocument ServeResponse::parseResp(const QString &resp){

    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    return _doc;

}

QString ServeResponse::getStringMember(QJsonDocument &doc, const QString &key){

    if(doc.isObject())
    {
        QJsonObject obj = doc.object();
        auto _message = obj.find(key);

        if(_message->isString()){
            return _message.value().toString();
        }
    }

    return "";
}

QString ServeResponse::getStringMember(QJsonObject &obj, const QString &key){

    auto _message = obj.find(key);

    if(_message->isString()){
        return _message.value().toString();
    }

    return "";
}
