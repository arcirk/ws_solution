#include "../include/server_response.h"

ServeResponse::ServeResponse(const std::string& resp)
{
    isParse = false;
    parse(resp);
}

std::string ServeResponse::base64_decode(const std::string &resp)
{
    std::string json;
    try {
        json = arcirk::base64_decode(resp);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    return json;
}

void ServeResponse::parse(const std::string& resp){


    arcirk::bJson _doc = arcirk::bJson();
    _doc.parse(resp);
    if(!_doc.is_parse())
    {
        if(_doc.IsObject())
        {
            auto _message = _doc.get_member("message");
            auto _result = _doc.get_member("result");
            auto _command = _doc.get_member("command");
            auto _recipient = _doc.get_member("uuid_channel");
            auto _uuid = _doc.get_member("user_uuid");
            auto _uuid_session= _doc.get_member("uuid");
            auto _contentType = _doc.get_member("contentType");
            auto _recipientName = _doc.get_member("channel_name");
            auto _app_name = _doc.get_member("app_name");

            if(_message.is_string()){
                message = _message.get_string();
            }
            if(_command.is_string()){
                command = _command.get_string();
            }
            if(_result.is_string()){
                result = _result.get_string();
            }
            if(_recipient.is_string()){
                recipient = _recipient.get_string();
            }
            if(_uuid.is_string()){
                uuid = _uuid.get_string();
            }
            if(_contentType.is_string()){
                contentType = _contentType.get_string();
            }
            if(_contentType.is_string()){
                recipientName = _recipientName.get_string();
            }
            if(_app_name.is_string()){
                app_name = _app_name.get_string();
            }
            if(_uuid_session.is_string()){
                uuid_session = _uuid_session.get_string();
            }
            isParse = true;
        }
    }

}

std::string ServeResponse::to_string() const
{
    arcirk::bJson msg = arcirk::bJson();
    msg.set_object();
    msg.addMember(arcirk::content_value("message", message));
    msg.addMember(arcirk::content_value("command", command));
    msg.addMember(arcirk::content_value("result", result));
    msg.addMember(arcirk::content_value("recipient", recipient));
    msg.addMember(arcirk::content_value("token", token));
    msg.addMember(arcirk::content_value("uuid", uuid));
    msg.addMember(arcirk::content_value("contentType", contentType));
    msg.addMember(arcirk::content_value("recipientName", recipientName));
    msg.addMember(arcirk::content_value("app_name", app_name));
    msg.addMember(arcirk::content_value("uuid_session", uuid_session));
    return msg.to_string();
}

arcirk::bJson ServeResponse::parseResp(const std::string &resp){

    arcirk::bJson _doc = arcirk::bJson();
    _doc.parse(resp);
    return _doc;

}

