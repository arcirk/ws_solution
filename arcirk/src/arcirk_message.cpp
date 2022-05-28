//
// Created by arcady on 04.11.2021.
//

#include "../include/arcirk.h"

namespace arcirk{

    //namespace websockets{

        _ws_message::_ws_message()
                : uuid(nil_uuid())
                , name("noname")
                , uuid_channel(nil_uuid())
                , message("")
                , uuid_form(nil_uuid())
                , command("")
                , hash("")
                , app_name("unknown")
                , result("ok")
                , role("user")
                , presentation("noname")
                , uuid_user(nil_uuid())
                , contentType("HTML")
                , object_name("")
                , msg_ref(nil_string_uuid())
                , ip_address("0.0.0.0")
                , host_name(boost::asio::ip::host_name())
        {
        }
        std::vector<std::string> _ws_message::get_fields(){
            return {"uuid", "name", "uuid_channel", "message", "uuid_form", "command"
                    , "hash", "app_name", "result", "role", "presentation", "uuid_user", "object_name", "host_name"};
        }
        void _ws_message::set_value(const std::string& nameField, bVariant& val){

            setFun f = get_set_function(nameField);
            if (f)
                f(val);
        }

        void _ws_message::set_uuid(bVariant& val){
            if (val.is_uuid())
                uuid = val.get_uuid();
        }
        void _ws_message::set_uuid_user(bVariant& val){
            if (val.is_uuid())
                uuid_user = val.get_uuid();
        }
        void _ws_message::set_uuid_form(bVariant& val){
            if (val.is_uuid())
                uuid_form = val.get_uuid();
        }
        void _ws_message::set_uuid_channel(bVariant& val){
            if (val.is_uuid())
                uuid_form = val.get_uuid();
        }
        void _ws_message::set_name(bVariant& val){
            if (val.is_string())
                name = val.get_string();
        }
        void _ws_message::set_message(bVariant& val){
            if (val.is_string())
                message = val.get_string();
        }
        void _ws_message::set_command(bVariant& val){
            if (val.is_string())
                command = val.get_string();
        }
        void _ws_message::set_hash(bVariant& val){
            if (val.is_string())
                hash = val.get_string();
        }
        void _ws_message::set_app_name(bVariant& val){
            if (val.is_string())
                app_name = val.get_string();
        }
        void _ws_message::set_result(bVariant& val){
            if (val.is_string())
                result = val.get_string();
        }
        void _ws_message::set_role(bVariant& val){
            if (val.is_string())
                role = val.get_string();
        }
        void _ws_message::set_presentation(bVariant& val){
            if (val.is_string())
                presentation = val.get_string();
        }
        void _ws_message::set_contentType(bVariant& val){
            if (val.is_string())
                contentType = val.get_string();
        }
        void _ws_message::set_channel_name(bVariant& val){
            if (val.is_string())
                channel_name = val.get_string();
        }
        void _ws_message::set_object_name(bVariant& val){
            if (val.is_string())
                object_name = val.get_string();
        }
        void _ws_message::set_msg_ref(bVariant& val){
            if (val.is_string())
                msg_ref = val.get_string();
        }
        void _ws_message::set_ip_address(bVariant& val){
            if (val.is_string())
                ip_address = val.get_string();
        }
        void _ws_message::set_host_name(bVariant& val){
            if (val.is_string())
                host_name = val.get_string();
        }
        setFun _ws_message::get_set_function(const std::string& nameField){
            if (nameField == "uuid")
            {
                return std::bind(&_ws_message::set_uuid, this, std::placeholders::_1);
            }else if (nameField == "uuid_user")
            {
                return std::bind(&_ws_message::set_uuid_user, this, std::placeholders::_1);
            }else if (nameField == "uuid_form")
            {
                return std::bind(&_ws_message::set_uuid_form, this, std::placeholders::_1);
            }else if (nameField == "uuid_channel")
            {
                return std::bind(&_ws_message::set_uuid_channel, this, std::placeholders::_1);
            }else if (nameField == "presentation")
            {
                return std::bind(&_ws_message::set_presentation, this, std::placeholders::_1);
            }else if (nameField == "role")
            {
                return std::bind(&_ws_message::set_role, this, std::placeholders::_1);
            }else if (nameField == "result")
            {
                return std::bind(&_ws_message::set_result, this, std::placeholders::_1);
            }else if (nameField == "app_name")
            {
                return std::bind(&_ws_message::set_app_name, this, std::placeholders::_1);
            }else if (nameField == "hash")
            {
                return std::bind(&_ws_message::set_hash, this, std::placeholders::_1);
            }else if (nameField == "command")
            {
                return std::bind(&_ws_message::set_command, this, std::placeholders::_1);
            }else if (nameField == "message")
            {
                return std::bind(&_ws_message::set_message, this, std::placeholders::_1);
            }else if (nameField == "name")
            {
                return std::bind(&_ws_message::set_name, this, std::placeholders::_1);
            }else if (nameField == "contentType")
            {
                return std::bind(&_ws_message::set_contentType, this, std::placeholders::_1);
            } else if (nameField == "channel_name")
            {
                return std::bind(&_ws_message::set_channel_name, this, std::placeholders::_1);
            } else if (nameField == "object_name")
            {
                return std::bind(&_ws_message::set_object_name, this, std::placeholders::_1);
            }else if (nameField == "msg_ref")
            {
                return std::bind(&_ws_message::set_msg_ref, this, std::placeholders::_1);
            }else if (nameField == "ip_address")
            {
                return std::bind(&_ws_message::set_ip_address, this, std::placeholders::_1);
            }else if (nameField == "host_name")
            {
                return std::bind(&_ws_message::set_host_name, this, std::placeholders::_1);
            }else
                return nullptr;

        }


        ws_message::ws_message() {
            m_message = _ws_message();
            m_doc = bJson();
        }

        ws_message::ws_message(_ws_message &defMessage) {
            m_message = defMessage;
        }

        ws_message::ws_message(const std::string &jsonText) {
            {
                m_doc.parse(jsonText);

                if (is_parse()) {
                    for (const std::string& fName: _ws_message::get_fields()) {
                        bVariant value;
                        if (m_doc.getMember(fName, value)) {
                            m_message.set_value(fName, value);
                        }
                    }
                }
            }
        }

        std::string ws_message::get_json(bool to_base64_){
            m_doc = bJson();
            m_doc.set_object();
            m_doc.addMember(content_value("uuid", m_message.uuid));
            m_doc.addMember(content_value("name", m_message.name));
            m_doc.addMember(content_value("uuid_channel", m_message.uuid_channel));
            m_doc.addMember(content_value("message", m_message.message));
            m_doc.addMember(content_value("uuid_form", m_message.uuid_form));
            m_doc.addMember(content_value("command", m_message.command));
            m_doc.addMember(content_value("result", m_message.result));
            m_doc.addMember(content_value("app_name", m_message.app_name));
            m_doc.addMember(content_value("role", m_message.role));
            m_doc.addMember(content_value("user_uuid", m_message.uuid_user));
            m_doc.addMember(content_value("contentType", m_message.contentType));
            m_doc.addMember(content_value("channel_name", m_message.channel_name));
            m_doc.addMember(content_value("object_name", m_message.object_name));
            m_doc.addMember(content_value("msg_ref", m_message.msg_ref));
            m_doc.addMember(content_value("ip_address", m_message.ip_address));
            m_doc.addMember(content_value("host_name", m_message.host_name));

            std::string result = m_doc.to_string();

            if (!to_base64_)
                return result;
            else
                return base64_encode(result);
        }

        bool ws_message::is_parse() {
            return m_doc.is_parse();
        }

        std::string ws_message::to_string() {
            return get_json(false);
        }

        _ws_message& ws_message::message() {
            return m_message;
        }
    //}

}
