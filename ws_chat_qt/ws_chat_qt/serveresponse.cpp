#include "serveresponse.h"

#include <iws_client.h>

ServeResponse::ServeResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

QString ServeResponse::base64_decode(const std::string &resp)
{
    std::string json;
    try {
        json = arc_json::base64_decode(resp);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    return QString::fromStdString(json);
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
