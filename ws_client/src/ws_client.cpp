//
// Created by arcady on 13.07.2021.
//

#include "../include/net.h"
#include "../include/ws_client.h"
#include "../include/ws_session.h"

#include <utility>


#include <boost/locale.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/locale/generator.hpp>

//ws_client::ws_client(net::io_context &io_context, const std::string& uuid, const std::string& name, const std::string& pwd, const std::string& app_name, const std::string& uuid_form, const std::string& user_uuid)
ws_client::ws_client(net::io_context &io_context, const std::string& client_param)
: ioc(io_context)
{
    _started = false;
    decode_message = false;
    _client_param = client_param;

//    if (uuid.empty()){
        set_uuid();
//    }else
//        set_uuid(uuid);
//
//    if (user_uuid.empty())
        set_user_uuid();
//    else
//        set_user_uuid(user_uuid);
//
//    if (name.empty())
        set_name("anonymous");
//    else
//        set_name(name);
//

//
//    if (app_name.empty())
        _app_name = "unknown";
//    else
//        _app_name = app_name;
//
//    _hash = arc_json::get_hash(name, pwd);
//
//    if (!uuid_form.empty())
//        _uuid_form = "00000000-0000-0000-0000-000000000000";
//    else
//        _uuid_form = uuid_form;


}
void
ws_client::
to_channel(const std::string &message, const std::string &uuid_sub, const std::string &uuid_form) {

    std::string msg = "cmd message ";
    msg.append(uuid_form + " ");
    //msg.append(arc_json::get_message(get_uuid(), message, get_name(), "", true, get_app_name(), uuid_form));
}

void
ws_client::
send(const std::string &message, bool is_cmd, const std::string& sub_user_uuid, const std::string& uuid_form, const std::string& command)
{

//    std::string _message;
//    bool is_cmd = false;
//
//    if (arc_json::is_cmd(message)){
//        if (format_cmd){
//            _message = arc_json::format_command_message(message);
//            if (_message == "error")
//                return;
//        }
//        is_cmd = true;
//    }


    std::string _uuid_form = uuid_form;
    if (_uuid_form.empty())
        _uuid_form = "00000000-0000-0000-0000-000000000000";
    std::string _sub_user_uuid = sub_user_uuid;
    if (_sub_user_uuid.empty())
        _sub_user_uuid = "00000000-0000-0000-0000-000000000000";

    boost::uuids::uuid  uuid_channel = arc_json::string_to_uuid(_sub_user_uuid);

    std::string msg;

    if (is_cmd){
        msg.append("cmd ");
    } else{
        msg.append("msg " + _sub_user_uuid + " ");
    }

    msg.append(arc_json::get_message(get_uuid(), message, get_name(), uuid_channel, true, get_app_name(), _uuid_form, "", command));

    auto const ss = boost::make_shared<std::string const>(std::move(msg));

    std::vector<boost::weak_ptr<session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->send(ss);
}

void
ws_client::
open(const char *host, const char *port) {
    boost::make_shared<session>(ioc)->run(host, port, this);
    ioc.run();
}

void
ws_client::
open(const char* host, const char* port, _callback_message& msg) {

    _callback_msg = msg;

    boost::make_shared<session>(ioc)->run(host, port, this);
    ioc.run();

}

void
ws_client::
open(const char *host, const char *port, const char *name) {
    boost::make_shared<session>(ioc)->run(host, port, this);
    set_name(name);
    ioc.run();
}

void
ws_client::
open(const char *host, const char *port, const char *name, const char *uuid) {
    boost::make_shared<session>(ioc)->run(host, port, this);
    set_name(name);
    set_uuid(uuid);
    ioc.run();
}

void
ws_client::
on_connect(session * sess){

    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(sess);
    _started = true;
    boost::uuids::uuid  uuid_channel{};

    if (_callback_msg)
    {
        std::string msg = arc_json::get_message(get_uuid(), "client connection success", get_name(), uuid_channel, true, _app_name);
        _callback_msg(msg);
    }

    if (!_client_param.empty())
        send_command("set_client_param", "", _client_param, sess);

//    //синхронизируем id и имя на сервере
//    std::string command = "cmd ";
//    command.append(arc_json::get_message(
//            get_uuid(),
//            "set_client_param",
//            get_name(),
//            uuid_channel,
//            true,
//            _app_name,
//            _uuid_form,
//            _hash,
//            "set_client_param",
//            "",
//            "",
//            get_user_uuid()));
//
//    auto const ss = boost::make_shared<std::string const>(std::move(command));
//    sess->send(ss);

    concole_log("log: client connection success");
}

void
ws_client::on_stop() {

    if (_callback_msg)
    {
        _started = false;
        boost::uuids::uuid  uuid_channel{};
        std::string msg = arc_json::get_message(get_uuid(), "client leave", get_name(), uuid_channel, true);
        _callback_msg(msg);

    }

    concole_log("log: client on_stop");
}

void
ws_client::
close() {

    _started = false;

    std::vector<boost::weak_ptr<session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->stop();

}

bool&
ws_client::
started() {
    return _started;
}

boost::uuids::uuid&
ws_client::
get_uuid() {
    return uuid_;
}

void
ws_client::
set_uuid() {
    uuid_ = boost::uuids::random_generator()();
}

void
ws_client::
set_uuid(const std::string& uuid) {
    //boost::uuids::string_generator gen;
    //uuid_ = gen(uuid);
    arc_json::is_valid_uuid(uuid, uuid_);
}

void
ws_client::
set_user_uuid(const std::string& uuid) {
    arc_json::is_valid_uuid(uuid, _user_uuid);
}

std::string &ws_client::get_name() {
    return name_;
}

void ws_client::set_name(std::string name) {
    name_ = std::move(name);
}

void ws_client::set_param(ptree &pt) {
    try {
        std::string uuid = arc_json::get_member(pt, "uuid");
        if (!uuid.empty())
            set_uuid(uuid);
        std::string user_uuid = arc_json::get_member(pt, "user_uuid");
        if (!user_uuid.empty())
            set_user_uuid(user_uuid);
        std::string name = arc_json::get_member(pt, "name");
        if (!name.empty())
            set_name(name);

    }catch (std::exception&){
        _callback_msg("Ошибка установки параметров сеанса!");
    }
}

void
ws_client::on_read(const std::string& message) {

    //std::string result = message;

    if (message == "\n" || message.empty() || message == "pong")
        return;

    arc_json::T_vec v = arc_json::split(message, "\n");

    std::string msg;

    if (!v.empty())
        msg = v[0];

    //если перед сообщением маркер "result" - это приватный ответ сервера клиенту
    if (v[0].substr(0, 6) == "result"){

        std::string base64 = v[0].substr(7 , v[0].length() - 6);
        msg = base64;

        try {
            std::string result = arc_json::base64_decode(base64);
            ptree pt;
            if (arc_json::parse(result, pt)){
                if (arc_json::get_member(pt, "command") == "set_client_param"){
                    set_param(pt);

                }
            }
        }catch (std::exception& e){
            std::cerr << e.what() << std::endl;
            _callback_msg(e.what());
            return;
        }

    }

    try {
        if (_callback_msg)
        {
            if (!decode_message){
                _callback_msg(msg);
            } else{
                std::string result = arc_json::base64_decode(msg);
                _callback_msg(result);
            }

        }
    }
    catch (std::exception& e){
        std::cout << "error: " << e.what() << std::endl;
        std::cout << "msg: {" << message << "}" << std::endl;
        return;
    }

}

std::string ws_client::get_client_info() {

    boost::uuids::uuid  uuid_channel{};

    std::string msg = arc_json::get_message(get_uuid(), "get_client_info", get_name(), uuid_channel, true);

    return msg;
}

void ws_client::error(const std::string &what, const std::string &err) {
    if (_callback_msg)
    {
        std::string _err(err);

        //#ifdef _WINDOWS
        //
        //        boost::locale::generator g;
        //        g.locale_cache_enabled(true);
        //        std::locale loc = g(boost::locale::util::get_system_locale());
        //        _err = boost::locale::conv::to_utf<char>(err, loc);
        //
        //        //using namespace boost::locale::conv;
        //        //_err = utf_to_utf<char>(err);
        //
        //#endif // _WINDOWS

        std::string desc = "error ";
        desc.append(what);
        desc.append(": ");
        desc.append(_err);

        boost::uuids::uuid  uuid_channel{};

        std::string msg = arc_json::get_message(get_uuid(), desc, get_name(), uuid_channel, true, get_app_name(), "", "", "", "error");

        _callback_msg(msg);
        //_callback_msg("error");
    }
}

//std::string ws_client::create_message(const std::string &uuid, const std::string &msg, const std::string &name,
//                                      const std::string &uuid_channel, const std::string &app_name,
//                                      const std::string &uuid_form, const std::string &hash, const std::string &command,
//                                      const std::string& role, const std::string &result, const std::string &user_uuid) {
//
//    try{
//        if (uuid.empty()){
//            return "error";
//        }
//
//        boost::uuids::uuid _uuid{};
//
//        if (!arc_json::is_valid_uuid(uuid, _uuid)){
//            _uuid = boost::uuids::random_generator()();
//        }
//        boost::uuids::uuid uuid_channel_{};
//        if (!uuid_channel.empty())
//            uuid_channel_ = arc_json::string_to_uuid(uuid_channel);
//
//        boost::uuids::uuid user_uuid_{};
//        if (!user_uuid.empty())
//            user_uuid_ = arc_json::string_to_uuid(user_uuid);
//
//        return arc_json::get_message(_uuid,
//                                     msg,
//                                     name,
//                                     uuid_channel_,
//                                     true,
//                                     app_name,
//                                     uuid_form,
//                                     hash,
//                                     command,
//                                     result,
//                                     role,
//                                     user_uuid_);
//    }catch (std::exception & e){
//
//        return e.what();
//    }
//
//}

boost::uuids::uuid& ws_client::get_user_uuid() {
    return _user_uuid;
}

void ws_client::set_user_uuid() {

    _user_uuid = boost::uuids::random_generator()();

}

void ws_client::subscribe_to_channel(const std::string &uuid_channel, const std::string &uuid_form) {

    boost::uuids::uuid uuid{};
    arc_json::is_valid_uuid(uuid_channel, uuid);
    std::string msg = arc_json::get_message(get_uuid(), "subscribe_to_channel", get_name(), uuid, true, get_app_name(), uuid_form, "", "subscribe_to_channel");
    send("cmd " + msg, false);
}

void ws_client::subscribe_exit_channel(const std::string &uuid_channel, const std::string &uuid_form) {

    boost::uuids::uuid uuid{};
    arc_json::is_valid_uuid(uuid_channel, uuid);
    std::string msg = arc_json::get_message(get_uuid(), "subscribe_close_channel", get_name(), uuid, true, get_app_name(), uuid_form, "", "subscribe_close_channel");
    send("cmd " + msg, false);
}

std::string &ws_client::get_app_name() {
    return _app_name;
}

void ws_client::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param, session * sess) {

        std::string _uuid_form = uuid_form;
//
//        if (cmd.empty())
//            return;
        if (_uuid_form.empty())
            _uuid_form = "00000000-0000-0000-0000-000000000000";

//        std::string result = cmd;
//        result.append(" ");
//        result.append(_uuid_form);
//
//        if (!param.empty()){
//            std::string cmd_param = arc_json::parse_param(param);
//            if (!cmd_param.empty()){
//                result.append(" ");
//                result.append(cmd_param);
//            }
//        }

        boost::uuids::uuid ch = boost::uuids::nil_uuid();

        std::string msg = "cmd " + arc_json::get_message(get_uuid(), param, get_name(), ch, true, get_app_name(), _uuid_form, "", cmd);

        auto const ss = boost::make_shared<std::string const>(std::move(msg));

        sess->send(ss);
}

void ws_client::set_session_param(const std::string &uuid, const std::string &name, const std::string &pwd,
                                  const std::string &app_name, const std::string &user_uuid) {
    std::string result = "set_client_param ";
    result.append("00000000-0000-0000-0000-000000000000 "); //форма
    result.append(uuid + " ");
    result.append(name + " ");
    result.append(arc_json::get_hash(name, pwd) + " ");
    result.append(app_name + " ");
    result.append(user_uuid + " ");

    std::string msg = "cmd " + arc_json::get_message(get_uuid(), result);

    send(msg, false);
}


