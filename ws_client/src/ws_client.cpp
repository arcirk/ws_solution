//
// Created by arcady on 13.07.2021.
//

#include "../include/net.h"
#include "../include/ws_client.h"
#include "../include/ws_session.h"

#include <utility>


#include <boost/locale.hpp>
//#include <boost/lexical_cast.hpp>
#include <boost/locale/generator.hpp>

using namespace arcirk;

ws_client::ws_client(net::io_context &io_context, const std::string& client_param)
: ioc(io_context)
{
    //_started = false;
    decode_message = false;
    _client_param = client_param;
    set_uuid();
    set_user_uuid();
    set_name("anonymous");
    _app_name = "unknown";
    _is_login = false;
}
//void
//ws_client::
//to_channel(const std::string &message, const std::string &uuid_sub, const std::string &uuid_form) {
//
//    std::string msg = "cmd message ";
//    msg.append(uuid_form + " ");
//    //msg.append(arc_json::get_message(get_uuid(), message, get_name(), "", true, get_app_name(), uuid_form));
//}

void
ws_client::
send(const std::string &message, bool is_cmd, const std::string& sub_user_uuid, const std::string& uuid_form, const std::string& command)
{


//    std::string _uuid_form = uuid_form;
//    if (_uuid_form.empty())
//        _uuid_form = "00000000-0000-0000-0000-000000000000";
    std::string _sub_user_uuid = sub_user_uuid;
    if (_sub_user_uuid.empty())
        _sub_user_uuid = "00000000-0000-0000-0000-000000000000";

    boost::uuids::uuid  uuid_channel = arcirk::string_to_uuid(_sub_user_uuid);

    std::string msg;

    if (is_cmd){
        msg.append("cmd ");
    } else{
        msg.append("msg " + _sub_user_uuid + " ");
    }

    auto _msg = ws_message();
    _msg.message().uuid = get_uuid();
    _msg.message().message = message;
    _msg.message().name = get_name();
    _msg.message().uuid_channel = uuid_channel;
    _msg.message().app_name = get_app_name();
    _msg.message().command = command;
    _msg.message().uuid_form = arcirk::string_to_uuid(uuid_form, false);
    _msg.message().uuid_user = get_user_uuid();

    msg.append(_msg.get_json(true));

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
    _is_login = false;
    ioc.run();
}

void
ws_client::
open(const char* host, const char* port, _callback_message& msg) {

    _is_login = false;

    _callback_msg = msg;

    boost::make_shared<session>(ioc)->run(host, port, this);
    ioc.run();

}

void
ws_client::
open(const char *host, const char *port, const char *name) {
    boost::make_shared<session>(ioc)->run(host, port, this);
    _is_login = false;
    set_name(name);
    ioc.run();
}

void
ws_client::
open(const char *host, const char *port, const char *name, const char *uuid) {
    boost::make_shared<session>(ioc)->run(host, port, this);
    set_name(name);
    set_uuid(uuid);
    _is_login = false;
    ioc.run();
}

void
ws_client::
on_connect(session * sess){

    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(sess);
    //_started = true;

    if (_callback_msg)
    {
        auto _msg = ws_message();
        _msg.message().uuid = get_uuid();
        _msg.message().message = "client connection success";
        _msg.message().name = get_name();
        _msg.message().app_name = _app_name;
        _msg.message().command = "connect_unknown_user";

        std::string msg = _msg.get_json(true);
        _callback_msg(msg);
    }

    if (!_client_param.empty())
        send_command("set_client_param", "", _client_param, sess);

    if(status_changed)
        status_changed(started());
}

void
ws_client::on_stop() {

    if (_callback_msg)
    {
        //_started = false;
        auto _msg = ws_message();
        _msg.message().uuid = get_uuid();
        _msg.message().message = "client leave";
        _msg.message().name = get_name();
        _msg.message().app_name = get_app_name();
        _msg.message().command = "close_connections";

        _callback_msg(_msg.get_json(true));

    }
    if(status_changed)
        status_changed(false);

    console_log("log: client on_stop");
}

void
ws_client::
close() {

    //_started = false;

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

//    if(status_changed)
//        status_changed(started());
}

bool
ws_client::
started() {

    //bool _started = false;

//    std::vector<boost::weak_ptr<session>> v;
//    {
//        std::lock_guard<std::mutex> lock(mutex_);
//        v.reserve(sessions_.size());
//        for(auto p : sessions_)
//            v.emplace_back(p->weak_from_this());
//    }
//
//    for(auto const& wp : v)
//        if(auto sp = wp.lock()){
//            if (sp->is_open()){
//                _started = true;
//                break;
//            }
//        }

        for(auto p : sessions_){
            if (p->is_open()){
                return true;
                //break;
            }
        }
    return false;
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
    is_valid_uuid(uuid, uuid_);
}

void
ws_client::
set_user_uuid(const std::string& uuid) {
    is_valid_uuid(uuid, _user_uuid);
}

std::string &ws_client::get_name() {
    return name_;
}

void ws_client::set_name(std::string name) {
    name_ = std::move(name);
}

void ws_client::set_param(ptree &pt) {
    try {
        std::string uuid = bJson::get_pt_member(pt, "uuid");
        if (!uuid.empty())
            set_uuid(uuid);
        std::string user_uuid = bJson::get_pt_member(pt, "user_uuid");
        if (!user_uuid.empty())
            set_user_uuid(user_uuid);
        std::string name = bJson::get_pt_member(pt, "name");
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

    T_vec v = split(message, "\n");

    std::string msg;

    if (!v.empty())
        msg = v[0];

    //если перед сообщением маркер "result" - это приватный ответ сервера клиенту
    if (v[0].substr(0, 6) == "result"){

        std::string base64 = v[0].substr(7 , v[0].length() - 6);
        msg = base64;

        try {
            std::string result = base64_decode(base64);
            ptree pt;
            if (bJson::parse_pt(result, pt)){
                if (bJson::get_pt_member(pt, "command") == "set_client_param"){
                    //авторизация прошла успешно, устанавливаем параметры сессии на клиенте
                    _is_login = true;
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
                std::string result = base64_decode(msg);
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

    auto _msg = ws_message();
    _msg.message().uuid = get_uuid();
    _msg.message().message = "get_client_info";
    _msg.message().name = get_name();
    _msg.message().app_name = get_app_name();

    return _msg.get_json(true);
}

void ws_client::error(const std::string &what, const std::string &err) {
    if (_callback_msg)
    {
        auto _msg = ws_message();
        _msg.message().uuid = get_uuid();
        _msg.message().message = err;
        _msg.message().name = get_name();
        _msg.message().app_name = get_app_name();
        _msg.message().command = what;
        _msg.message().result = "error";

        _callback_msg(_msg.get_json(true));
    }
    if(status_changed)
        status_changed(started());
}

boost::uuids::uuid& ws_client::get_user_uuid() {
    return _user_uuid;
}

void ws_client::set_user_uuid() {

    _user_uuid = boost::uuids::random_generator()();

}

void ws_client::subscribe_to_channel(const std::string &uuid_channel, const std::string &uuid_form) {

    boost::uuids::uuid uuid{};
    is_valid_uuid(uuid_channel, uuid);
    auto _msg = ws_message();
    _msg.message().uuid = get_uuid();
    _msg.message().message = "subscribe_to_channel";
    _msg.message().name = get_name();
    _msg.message().app_name = get_app_name();
    _msg.message().command = "subscribe_to_channel";
    _msg.message().uuid_form = arcirk::string_to_uuid(uuid_form, false);

    send("cmd " + _msg.get_json(true), false);
}

void ws_client::subscribe_exit_channel(const std::string &uuid_channel, const std::string &uuid_form) {

    boost::uuids::uuid uuid{};
    is_valid_uuid(uuid_channel, uuid);
    auto _msg = ws_message();
    _msg.message().uuid = get_uuid();
    _msg.message().message = "subscribe_close_channel";
    _msg.message().name = get_name();
    _msg.message().app_name = get_app_name();
    _msg.message().command = "subscribe_close_channel";
    _msg.message().uuid_form = arcirk::string_to_uuid(uuid_form, false);

    send("cmd " + _msg.get_json(true), false);
}

std::string &ws_client::get_app_name() {
    return _app_name;
}

void ws_client::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param, session * sess) {

//        std::string _uuid_form = uuid_form;
//
//        if (cmd.empty())
//            return;
//        if (_uuid_form.empty())
//            _uuid_form = "00000000-0000-0000-0000-000000000000";

        auto _msg = ws_message();
        _msg.message().uuid = get_uuid();
        _msg.message().message = param;
        _msg.message().name = get_name();
        _msg.message().app_name = get_app_name();
        _msg.message().command = cmd;
        _msg.message().uuid_form = arcirk::string_to_uuid(uuid_form, false);

        std::string msg = "cmd " + _msg.get_json(true);

        auto const ss = boost::make_shared<std::string const>(std::move(msg));

        sess->send(ss);
}

void ws_client::set_session_param(const std::string &uuid, const std::string &name, const std::string &pwd,
                                  const std::string &app_name, const std::string &user_uuid) {
    std::string result = "set_client_param ";
    result.append("00000000-0000-0000-0000-000000000000 "); //форма
    result.append(uuid + " ");
    result.append(name + " ");
    result.append(get_hash(name, pwd) + " ");
    result.append(app_name + " ");
    result.append(user_uuid + " ");

    auto _msg = ws_message();
    _msg.message().uuid = get_uuid();
    _msg.message().message = result;

    std::string msg = "cmd " + _msg.get_json(true);

    send(msg, false);
}

void ws_client::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &json_param) {

    std::vector<boost::weak_ptr<session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            send_command(cmd, uuid_form, json_param, p);
    }


}

void ws_client::open(const char *host, const char *port, _callback_message &msg, _callback_status &st) {

    _is_login = false;

    _callback_msg = msg;
    status_changed = st;

    boost::make_shared<session>(ioc)->run(host, port, this);
    ioc.run();

}

void
ws_client::set_webdav_settings_on_client(const std::string &sett) {

    auto json = arcirk::bJson();
    json.parse(sett);

    _webdav_host = json.get_member("webdav_host").get_string();
    _webdav_user = json.get_member("webdav_user").get_string();
    _webdav_pwd = json.get_member("webdav_pwd").get_string();
    _webdav_ssl = json.get_member("webdav_pwd").get_bool();

}

void ws_client::set_webdav_directory(const std::string &dir) {

    _webdav_local_dir = dir;
}

void ws_client::use_webdav_local_dir(bool val) {
    _webdav_use_local = val;
}

std::string ws_client::get_webdav_user() const {
    return _webdav_user;
}

std::string ws_client::get_webdav_pwd() const {
    return _webdav_pwd;
}

std::string ws_client::get_webdav_host() const {
    return _webdav_host;
}
bool ws_client::get_webdav_ssl() {
    return _webdav_ssl;
}
