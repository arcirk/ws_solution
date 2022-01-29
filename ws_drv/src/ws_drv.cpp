/*
 *  Modern Native AddIn
 *  Copyright (C) 2018  Infactum
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifdef _WINDOWS
    #include "stdafx.h"
#endif // _WINDOWS

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#ifdef _WINDOWS
#include <iostream>
#include <thread>
#else
#include <boost/thread/thread.hpp>
#endif // _WINDOWS

#include "ws_drv.h"
//#include <ws_client.h>
//#include "server_response.h"

//#include <webdav/client.hpp>

//typedef std::function<void()> _start_func;

//using namespace arcirk;
//namespace dll = boost::dll;

std::string ws_drv::extensionName() {
    return "WebSocketClient";
}

ws_drv::ws_drv()
{

    //client = nullptr;

    settings = arcirk::bConf("conf_1c_client.json");

    if(app_name != "client_1C"){
        app_name = "client_1C";
//        settings[bConfFields::AppName] = app_name;
//        settings.save();
    }

    AddProperty(L"Version", L"Версия", [&]() {
        auto s = std::string(Version);
        return std::make_shared<variant_t>(std::move(s));
    });

    AddProperty(L"port", L"Порт", [&](){
        return std::make_shared<variant_t>(port);
    });
    AddProperty(L"host", L"Хост", [&](){
        return std::make_shared<variant_t>(host);
    });

    AddProperty(L"user_uuid", L"ИдентификаторПользователя", [&](){
        return std::make_shared<variant_t>(get_user_uuid());
    });
    AddProperty(L"app_uuid", L"ИдентификаторСессии", [&](){
        return std::make_shared<variant_t>(get_user_uuid());
    });

    AddMethod(L"getClientConf", L"ПолучитьКонфигурациюКлиента", this, &ws_drv::get_client_conf);
    AddMethod(L"saveClientConf", L"СохранитьКонфигурациюКлиента", this, &ws_drv::save_conf);
    AddMethod(L"Open", L"Открыть", this, &ws_drv::open);
    AddMethod(L"Close", L"Закрыть", this, &ws_drv::close);
//    AddMethod(L"Deliver", L"Сообщить", this, &ws_drv::send);
    AddMethod(L"Started", L"Запущен", this, &ws_drv::started);
    AddMethod(L"OpenAs", L"ОткрытьКак", this, &ws_drv::open_as);
    AddMethod(L"crypt", L"crypt", this, &ws_drv::crypt);
    AddMethod(L"CheckWebDav", L"ПроверитьWebDavПодключение", this, &ws_drv::webdav_check);

//    AddMethod(L"GetClientInfo", L"ПолучитьИнформациюОТекущемКлиенте", this, &ws_drv::get_client_info);
//    AddMethod(L"CurrentName", L"ТекущееИмя", this, &ws_drv::get_current_name);
//    //AddMethod(L"CloseChannel", L"ОтключитсяОтКанала", this, &ws_drv::close_channel);
//    //AddMethod(L"JoinChannel", L"ПрисоединитсяККаналу", this, &ws_drv::join_channel);
//    AddMethod(L"SendCommand", L"КомандаСерверу", this, &ws_drv::send_command);
    AddMethod(L"GetHash", L"ПолучитьХеш", this, &ws_drv::get_hash);
//    AddMethod(L"CurrentDate", L"ТекущаяДата", this, &ws_drv::currentDate);
//    AddMethod(L"CurrentDateSeconds", L"ТекущаяДатаВСекундах", this, &ws_drv::current_date_in_seconds);
//    AddMethod(L"GetTZOffset", L"ПолучитьСмещениеВременнойЗоны", this, &ws_drv::get_tz_offset);
//    AddMethod(L"GetMessages", L"ПолучитьСообщения", this, &ws_drv::get_messages);
//    AddMethod(L"GetUserInfo", L"ПолучитьИнформациюОПользователе", this, &ws_drv::get_user_info);
//    AddMethod(L"GetGroupList", L"ПолучитьСписокГрупп", this, &ws_drv::get_group_list);
//    AddMethod(L"AddGroup", L"ДобавитьГруппу", this, &ws_drv::add_group);
//    AddMethod(L"EditGroup", L"ИзменитьГруппу", this, &ws_drv::edit_group);
//    AddMethod(L"RemoveGroup", L"УдалитьГруппу", this, &ws_drv::remove_group);
//    AddMethod(L"GetUsers", L"ПолучитьПользователей", this, &ws_drv::get_users);
//    AddMethod(L"SetParent", L"УстановитьГруппу", this, &ws_drv::set_parent);
//    AddMethod(L"RemoveUser", L"УдалитьПользователя", this, &ws_drv::remove_user);


}

void ws_drv::message(const variant_t &msg) {
    std::visit(overloaded{
        [&](const std::string &v) { AddError(ADDIN_E_INFO, extensionName(), v, false); },
        [&](const int32_t &v) {
            AddError(ADDIN_E_INFO, extensionName(), std::to_string(static_cast<int>(v)), false);
            },
            [&](const double &v) { AddError(ADDIN_E_INFO, extensionName(), std::to_string(v), false); },
            [&](const bool &v) {
            AddError(ADDIN_E_INFO, extensionName(), std::string(v ? u8"Истина" : u8"Ложь"), false);
            },
            [&](const std::tm &v) {
            std::ostringstream oss;
            oss.imbue(std::locale("ru_RU.utf8"));
            oss << std::put_time(&v, "%c");
            AddError(ADDIN_E_INFO, extensionName(), oss.str(), false);
            },
            [&](const std::vector<char> &v) {},
            [&](const std::monostate &) {}
            }, msg);
}

//void ws_drv::start(){
//
//    _callback_message callback = std::bind(&ws_drv::ext_message, this, std::placeholders::_1);
//
//    std::string _port = std::to_string(std::get<int32_t>(*port));
//    std::string _host = std::get<std::string>(*server);
//
//    auto const host = _host.c_str();
//    auto const port = _port.c_str();
//
//    boost::asio::io_context ioc;
//
//    close();
//
//    client = new ws_client(ioc);
//    client->open(host, port, callback);
//
//}
//
//void ws_drv::start_() {
//
//    _callback_message callback = std::bind(&ws_drv::ext_message, this, std::placeholders::_1);
//
//    std::string _port = std::to_string(std::get<int32_t>(*port));
//    std::string _host = std::get<std::string>(*server);
//
//    auto const host = _host.c_str();
//    auto const port = _port.c_str();
//
//    boost::asio::io_context ioc;
//
//    close();
//
//    std::string _param = std::get<std::string>(_client_param);
//
//    client = new ws_client(ioc, _param);
//
//    client->open(host, port, callback);
//
//}
//
//void ws_drv::close() {
//
////    if (client)
////    {
////        if (client->started())
////        {
////            client->close();
////        }
////
////        delete client;
////        client = nullptr;
////    }
//
//}
//
//int ws_drv::open()
//{
////#ifdef _WINDOWS
////    std::thread(std::bind(&ws_drv::start, this)).detach();
////#else
////    boost::thread(boost::bind(&ws_drv::start, this)).detach();
////#endif
////
//    return EXIT_SUCCESS;
//
//}
//
//int ws_drv::open_as(const variant_t &param) {
//
////    _client_param = param;
////
////#ifdef _WINDOWS
////    std::thread(std::bind(&ws_drv::start_, this)).detach();
////#else
////    boost::thread(boost::bind(&ws_drv::start_, this)).detach();
////#endif
////
//    return EXIT_SUCCESS;
//
//}
//
//void ws_drv::send(const variant_t& msg, const variant_t& sub_user_uuid, const variant_t& uuid_form)
//{
////    std::string _msg = std::get<std::string>(msg);
////    std::string _uuid_form = std::get<std::string>(uuid_form);
////    std::string _sub_user_uuid = std::get<std::string>(sub_user_uuid);
////
////    if (_msg.empty())
////        return;
////    if (_uuid_form.empty())
////        _uuid_form = arcirk::nil_string_uuid();
////    if (_sub_user_uuid.empty())
////        _sub_user_uuid = arcirk::nil_string_uuid(;
////
////    if (client)
////    {
////        if (client->started())
////        {
////            client->send(_msg, false, _sub_user_uuid);
////        }
////    }
//}
//
//void ws_drv::send_command(const variant_t &cmd, const variant_t &uuid_form, const variant_t &param) {
//
////    if (client){
////
////        if (!started())
////            return;
////
////        std::string _cmd = std::get<std::string>(cmd);
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////        std::string _param = std::get<std::string>(param);
////
////        if (_cmd.empty())
////            return;
////        if (_uuid_form.empty())
////            _uuid_form = "00000000-0000-0000-0000-000000000000";
////
////        if (_param.empty())
////            _param = "{\"command\":\"" + _cmd + "\"}";
////
////        client->send(_param, true, "00000000-0000-0000-0000-000000000000", _uuid_form, _cmd);
////
////    }
//
//}

//void ws_drv::send_command_(const std::string &cmd, const std::string &uuid_form, const std::string &param) {

//    if (client){
//
//        if (!started())
//            return;
//
//        std::string _cmd = cmd;
//        std::string _uuid_form = uuid_form;
//        std::string _param = param;
//
//        if (_cmd.empty())
//            return;
//        if (_uuid_form.empty())
//            _uuid_form = "00000000-0000-0000-0000-000000000000";
//
//        if (_param.empty())
//            _param = "{\"command\":\"" + _cmd + "\"}";
//
//        client->send(_param, true, "00000000-0000-0000-0000-000000000000", _uuid_form, _cmd);
//    }

//}

//void ws_drv::to_channel(const variant_t &msg, const variant_t &uuid_sub, const variant_t &uuid_form) {
//
//    std::string _msg = std::get<std::string>(msg);
//    std::string _uuid_form = std::get<std::string>(uuid_form);
//    std::string _uuid_sub = std::get<std::string>(uuid_sub);
//
//    if (_msg.empty())
//        return;
//    if (_uuid_form.empty())
//        _uuid_form = "00000000-0000-0000-0000-000000000000";
//    if (_uuid_sub.empty())
//        _uuid_sub = "00000000-0000-0000-0000-000000000000";
//
//
//}

//bool ws_drv::started() {
////    bool result = false;
////    if (client){
////        result = client->started();
////    }
////    return result;
//    return false;
//}
//
//variant_t ws_drv::get_client_info() {
////    if (client){
////        return client->get_client_info();
////
////    }else
//        return "";
//}
//
//std::string ws_drv::get_current_name() {
////    if (client)
////        return client->get_name();
////    else
//        return "unknown";
//}
//
////void ws_drv::join_channel(const variant_t &uuid_sub, const variant_t &uuid_form) {
////    if (client){
////        try {
////            std::string uuid_channel(std::get<std::string>(uuid_sub));
////            std::string _uuid_form(std::get<std::string>(uuid_form));
////
////            client->subscribe_to_channel(uuid_channel.c_str(), _uuid_form.c_str());
////
////        }catch (std::exception & e){
////            set_log(std::string ("error: ") + e.what(), "/errors.log");
////        }
////
////    }
////}
//
////void ws_drv::close_channel(const variant_t &uuid_sub, const variant_t &uuid_form) {
////    if (client){
////        try {
////            std::string uuid_channel(std::get<std::string>(uuid_sub));
////            std::string _uuid_form(std::get<std::string>(uuid_form));
////
////            client->subscribe_exit_channel(uuid_channel, _uuid_form);
////
////        }catch (std::exception & e){
////            set_log(std::string ("error: ") + e.what(), "/errors.log");
////        }
////    }
////}
//
//void ws_drv::set_log(const std::string &msg, const std::string &filename) {
//    //    try {
//    //        std::string _log_path = std::get<std::string>(*log_path);
//    //        if (_log_path.empty())
//    //            return;
//    //        if (msg.empty())
//    //            return;
//    //
//    ////        std::ofstream f;
//    ////        f.open(_log_path + filename, std::ios::app);
//    ////        f << msg << "\n";
//    ////        f.close();
//    //    }catch (std::exception & e){
//    //        //
//    //    }
//}
//
std::string ws_drv::get_hash(const variant_t &usr, const variant_t &pwd) {

//    std::string _usr(std::get<std::string>(usr));
//    std::string _pwd(std::get<std::string>(pwd));
//
//    return arcirk::get_hash(_usr, _pwd);

    return "";

}

variant_t ws_drv::get_currentDate() {
    using namespace std;
    tm current{};
    time_t t = time(nullptr);
#ifdef _WINDOWS
    localtime_s(&current, &t);
#else
    localtime_r(&t, &current);
#endif
    return current;
}

variant_t ws_drv::get_current_date_in_seconds() {
    //return (int)arcirk::current_date_seconds();
    return 0;
}

variant_t ws_drv::get_tz_offset() {
//    return (int)arcirk::tz_offset();
    return 0;
}

//void ws_drv::get_messages(const variant_t &uuid_sub, const variant_t &start_date, const variant_t &end_date, const variant_t &limit, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_sub = std::get<std::string>(uuid_sub);
////
////        tm _start_date = std::get<tm>(start_date);
////        std::chrono::system_clock::time_point tp_start = std::chrono::system_clock::from_time_t(mktime(&_start_date));
////        long int int_start_date = std::chrono::duration_cast<std::chrono::seconds>(tp_start.time_since_epoch()).count();
////
////        tm _end_date = std::get<tm>(end_date);
////        std::chrono::system_clock::time_point tp_end = std::chrono::system_clock::from_time_t(mktime(&_end_date));
////        long int int_end_date = std::chrono::duration_cast<std::chrono::seconds>(tp_end.time_since_epoch()).count();
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("recipient", _uuid_sub);
////        pt.add("start_date", int_start_date);
////        pt.add("end_date", int_end_date);
////        pt.add("limit", std::get<int>(limit));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("get_messages", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//
//}
//
//void ws_drv::get_user_info(const variant_t &user_uuid, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_sub = std::get<std::string>(user_uuid);
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("user_uuid", _uuid_sub);
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("get_user_info", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void ws_drv::get_group_list(const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        send_command_("get_group_list", _uuid_form, "");
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void
//ws_drv::add_group(const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("name", std::get<std::string>(name));
////        pt.add("presentation", std::get<std::string>(presentation));
////        pt.add("parent", std::get<std::string>(uuid_parent));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("add_group", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void ws_drv::edit_group(const variant_t &uuid_group, const variant_t &name, const variant_t &presentation,
//                        const variant_t &uuid_parent, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("name", std::get<std::string>(name));
////        pt.add("presentation", std::get<std::string>(presentation));
////        pt.add("parent", std::get<std::string>(uuid_parent));
////        pt.add("ref", std::get<std::string>(uuid_group));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("edit_group", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void ws_drv::remove_group(const variant_t &uuid_group, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("ref", std::get<std::string>(uuid_group));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("remove_group", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//
//}
//
//void ws_drv::get_users(const variant_t &uuid_group, const variant_t &uuid_form) {
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("channel", std::get<std::string>(uuid_group));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("get_users", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void ws_drv::set_parent(const variant_t &user_uuid, const variant_t &uuid_group, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("parent", std::get<std::string>(uuid_group));
////        pt.add("user", std::get<std::string>(user_uuid));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("set_parent", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}
//
//void ws_drv::remove_user(const variant_t &user_uuid, const variant_t &uuid_form) {
//
////    boost::property_tree::ptree pt;
////
////    try {
////
////        std::string _uuid_form = std::get<std::string>(uuid_form);
////
////        pt.add("ref", std::get<std::string>(user_uuid));
////
////        std::stringstream _ss;
////        boost::property_tree::json_parser::write_json(_ss, pt);
////
////        send_command_("remove_user", _uuid_form, _ss.str());
////
////    }catch (std::exception& e){
////        message("error: " + std::string (e.what()));
////    }
//}

std::string  ws_drv::get_client_conf() {

//    return settings.to_string();

    return "";
}

std::string ws_drv::get_user_uuid() const {
//    if(client)
//        return arcirk::uuid_to_string(client->get_user_uuid());
//    else
//        return arcirk::nil_string_uuid();

    return "";
}

std::string ws_drv::get_app_uuid() const {
//    if (client)
//        return arcirk::uuid_to_string(client->get_uuid());
//    else
//        return arcirk::nil_string_uuid();

    return "";
}

void ws_drv::open_as(const variant_t &_host, const variant_t &_port, const variant_t &_user, const variant_t &_pwd,
                     variant_t _save_conf) {

//    host = std::get<std::string>(_host);
//    port = std::get<int>(_port);
//    admin_name = std::get<std::string>(_user);
//    std::string  pwd = std::get<std::string>(_pwd);
//    hash = arcirk::get_hash(admin_name, pwd);
//    bool save_conf = std::get<bool>(_save_conf);
//
//    settings[bConfFields::User] = admin_name;
//    settings[bConfFields::ServerHost] = host;
//    settings[bConfFields::ServerPort] = port;
//    settings[bConfFields::Hash] = hash;
//    if(save_conf)
//        settings.save();
//
//    open();

}
void ws_drv::open(){
    _open();
}

void ws_drv::_open(bool new_thread) {

//    app_uuid = random_uuid();
//    user_uuid = random_uuid();
//
//    boost::property_tree::ptree pt;
//
//    pt.add("uuid", app_uuid);
//    pt.add("name", admin_name);
//    pt.add("hash", hash);
//    pt.add("app_name", app_name);
//    pt.add("user_uuid", user_uuid);
//
//    std::stringstream _ss;
//    boost::property_tree::json_parser::write_json(_ss, pt);
//
//    _client_param = _ss.str();
//
//    if (new_thread){
//#ifdef _WINDOWS
//        std::thread(std::bind(&ws_drv::start, this)).detach();
//#else
//        boost::thread(boost::bind(&ws_drv::start, this)).detach();
//#endif
//    }else
//        start();
}


void ws_drv::start() {

//    boost::asio::io_context ioc;
//
//    close();
//
//    _callback_message callback = std::bind(&ws_drv::ext_message, this, std::placeholders::_1);
//    _callback_status callback_status = std::bind(&ws_drv::status_changed, this, std::placeholders::_1);
//
//    client = new ws_client(ioc, _client_param);
//
//    try {
//        client->open(host.c_str(), std::to_string(port).c_str(), callback, callback_status);
//    }
//    catch (std::exception& e){
//        std::cerr << "IClient::start::exception: " << e.what() <<std::endl;
//    }
//
//    std::cout << "IClient::start: exit client thread" << std::endl;
//
//    if(client){
//        delete client;
//        client = nullptr;
//    }

}

void ws_drv::ext_message(const std::string &msg)
{
//    std::string resp = ServeResponse::base64_decode(msg); arcirk::base64_decode(msg);
//
//    if(!resp.empty()){
//       processServeResponse(resp);
//    }
//


    //#ifdef _WINDOWS
    //    using namespace boost::locale::conv;
    //    std::string _msg = utf_to_utf<char>(msg);
    //    this->ExternalEvent("WebSocketAddIn", "message", _msg);
    //#else
    //this->ExternalEvent("WebSocketAddIn", "message", msg);
    //#endif // _WINDOWS

    //this->ExternalEvent("WebSocketAddIn", "message", msg);
}

void ws_drv::status_changed(bool status)
{
    connectedStatusChanged(status);
}

void ws_drv::processServeResponse(const std::string &jsonResp)
{
//    auto resp = new ServeResponse(jsonResp);
//
//    if(!resp->isParse){
//        delete resp;
//        return;
//    }
//    if(resp->result == "error"){
//
//        displayError(resp->command, resp->message);
//
//       //if(resp->command == "set_client_param")
//            //client->close();
//
//    }else
//    {
//        if(resp->command == "set_client_param"){
//            connectionSuccess();
//            send_command("set_content_type", "", "{\"content_type\":\"HTML\"}");
//        }else if (resp->command == "set_content_type"){
//            send_command("set_message_struct_type", "", "{\"struct_type\":\"DB\"}");
//        }else if (resp->command == "set_message_struct_type"){
//            send_command("get_webdav_settings", "", "");
//        }else if (resp->command == "get_users_catalog"){
//            //
//        }else if (resp->command == "get_user_cache"){
//            //
//        }else if(resp->command == "get_active_users"){
//            getActiveUsers(resp->message);
//        }else if (resp->command == "get_messages"){
//            setMessages(resp->message);
//        }else if (resp->command == "close_connections"){
//            closeConnection();
//        }else if (resp->command == "message"){
//            messageReceived(resp->message, resp->uuid, resp->recipient, resp->recipientName);
//        }else if (resp->command == "set_user_cache"){
//            //
//        }else if (resp->command == "get_user_info"){
//            userInfo(resp->message);
//        }else if (resp->command == "client_join"){
//            clientJoin(resp->message);
//        }else if(resp->command == "client_leave"){
//            clientLeave(resp->message);
//        }else if(resp->command == "get_user_status"){
//            requestUserStatus(resp->message);
//        }else if(resp->command == "reset_unread_messages"){
//            //
//        }else if(resp->command == "get_unread_messages"){
//            unreadMessages(resp->message);
//        }else if(resp->command == "command_to_qt_client"){
//            //
//        }else if(resp->command == "get_group_list"){
//            getGroupList(resp->message);
//        }else if(resp->command == "get_users"){
//            getListUsers(resp->message);
//        }else if(resp->command == "add_group"){
//            addGroupUsers(resp->message);
//        }else if(resp->command == "edit_group"){
//             editGroupUsers(resp->message);
//        }else if(resp->command == "remove_group"){
//            removeGroupUsers(resp->message);
//        }else if (resp->command == "add_user"){
//            addUser(resp->message);
//        }else if (resp->command == "remove_user"){
//            deleteUser(resp->message);
//        }else if (resp->command == "update_user"){
//            updateUser(resp->message);
//        }else if (resp->command == "set_parent"){
//            setUserParent(resp->message);
//        }else if(resp->command == "get_channel_token"){
//            //
//        }else if (resp->command == "get_webdav_settings"){
//            setWebDavSettingsToClient(resp->message);
//            emit("get_webdav_settings", resp->message);
//        }else if (resp->command == "set_webdav_settings"){
//            //обновились настройки webdav на сервере
//        }else if (resp->command == "command_to_qt_agent"){
//            //
//        }
//        else
//            std::cout << "Не известная команда: " << resp->command << std::endl;
//    }
//
//    delete resp;
}

void ws_drv::displayError(const std::string &what, const std::string &err) {
    emit("Error", what + ": " + err);
}

void ws_drv::connectionSuccess() {

}

void ws_drv::closeConnection() {

}

void ws_drv::connectedStatusChanged(bool status) {
    std::string _status = status ? "true" : "false";
    emit("connectedStatusChanged", _status);
}

void ws_drv::userInfo(const std::string &uuid) {

}

void ws_drv::setMessages(const std::string &resp) {

}

void ws_drv::messageReceived(const std::string &msg, const std::string &uuid, const std::string &recipient,
                             const std::string &recipientName) {

}

void ws_drv::requestUserStatus(const std::string &resp) {

}

void ws_drv::unreadMessages(const std::string &resp) {

}

void ws_drv::getGroupList(const std::string &resp) {

}

void ws_drv::get_messages(const std::string &uuid_sub, const long &start_date, const long &end_date, int &limit,
                          const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("recipient", uuid_sub);
//        pt.add("start_date", (int)start_date);
//        pt.add("end_date", (int)end_date);
//        pt.add("limit", limit);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("get_messages", uuid_form, _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_user_info(const std::string &_user_uuid, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("uuid_user", _user_uuid);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("get_user_info", uuid_form, _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_group_list(const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        send_command("get_group_list", uuid_form, "");
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::add_group(const std::string &name, const std::string &presentation, const std::string &uuid_parent,
                       const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("name", name);
//        pt.add("presentation", presentation);
//        pt.add("parent", uuid_parent);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("add_group", uuid_form, _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::edit_group(const std::string &uuid_group, const std::string &name, const std::string &presentation,
                        const std::string &uuid_parent, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("name", name);
//        pt.add("presentation", presentation);
//        pt.add("parent", uuid_parent);
//        pt.add("ref", uuid_group);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("edit_group", uuid_form, _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::remove_group(const std::string &uuid_group, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//        pt.add("ref", uuid_group);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("remove_group", uuid_form, _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_users(const std::string &uuid_group, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("channel", uuid_group);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("get_users", uuid_form, _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::set_parent(const std::string &_user_uuid, const std::string &uuid_group, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("parent", uuid_group);
//        pt.add("user", _user_uuid);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("set_parent", uuid_form, _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::remove_user(const std::string &_user_uuid, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("ref", _user_uuid);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("remove_user", uuid_form, _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::kill_session(const std::string &_user_uuid, const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("ref", _user_uuid);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("kill_session", uuid_form, _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param) {
//    if (client){
//
//        if (!client->started())
//            return;
//
//        const std::string& _cmd = cmd;
//        std::string _uuid_form = uuid_form;
//        std::string _param = param;
//
//        if (_cmd.empty())
//            return;
//        if (_uuid_form.empty())
//            _uuid_form = "00000000-0000-0000-0000-000000000000";
//
//        if (_param.empty())
//            _param = "{\"command\":\"" + _cmd + "\"}";
//
//        client->send(_param, true, "00000000-0000-0000-0000-000000000000", _uuid_form, _cmd);
//    }
}

bool ws_drv::started() {

    bool result = false;
//
//    if (client){
//        result = client->started();
//    }
    return result;

}

void ws_drv::set_app_name(const std::string &session_uuid, const std::string &new_app_name) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        pt.add("uuid_set", session_uuid);
//        pt.add("new_app_name", new_app_name);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("set_app_name", nil_string_uuid(), _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_users_catalog(const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        std::string _uuid_form = uuid_form;
//
//        if (uuid_form.empty()){
//            _uuid_form = nil_string_uuid();
//        }
//        pt.add("uuid_form", _uuid_form);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("get_users_catalog", nil_string_uuid(), _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_user_cache(const std::string &uuid_form) {
//    boost::property_tree::ptree pt;
//
//    try {
//
//        std::string _uuid_form = uuid_form;
//
//        if (uuid_form.empty()){
//            _uuid_form = nil_string_uuid();
//        }
//        pt.add("uuid_form", _uuid_form);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("get_user_cache", nil_string_uuid(), _ss.str());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::set_user_cache(const std::string &cache, const std::string &uuid_form) {

//    boost::property_tree::ptree pt;
//
//    try {
//
//        std::string _uuid_form = uuid_form;
//
//        if (uuid_form.empty()){
//            _uuid_form = nil_string_uuid();
//        }
//        pt.add("uuid_form", _uuid_form);
//        pt.add("cache", arcirk::base64_encode(cache));
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);
//
//        send_command("set_user_cache", nil_string_uuid(), _ss.str());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_user_status(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param) {

//    try {
//        auto json = arcirk::bJson();
//        if(param.empty()){
//            json.SetObject();
//            json.addMember("uuid_user", _user_uuid);
//        }
//        else{
//            json.parse(param);
//            if (json.is_parse()){
//                bVariant var;
//                if (!json.getMember("uuid_user", var))
//                    json.addMember("uuid_user", _user_uuid);
//            }
//        }
//
//        send_command("get_user_status", uuid_form, json.to_string());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_user_data(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param) {

//    try {
//        auto json = arcirk::bJson();
//        if(param.empty()){
//            json.SetObject();
//            json.addMember("uuid_user", _user_uuid);
//        }
//        else{
//            json.parse(param);
//            if (json.is_parse()){
//                bVariant var;
//                if (!json.getMember("uuid_user", var))
//                    json.addMember("uuid_user", _user_uuid);
//            }
//        }
//
//        send_command("get_user_data", uuid_form, json.to_string());
//
//    }catch (std::exception&){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::send(const std::string &msg, const std::string &sub_user_uuid, const std::string &uuid_form,
                  const std::string &objectName, const std::string &msg_ref) {
//    std::string _msg = msg;
//    std::string _uuid_form = uuid_form;
//    std::string _sub_user_uuid = sub_user_uuid;
//
//    if (_msg.empty())
//        return;
//    if (_uuid_form.empty())
//        _uuid_form = "00000000-0000-0000-0000-000000000000";
//    if (_sub_user_uuid.empty())
//        _sub_user_uuid = "00000000-0000-0000-0000-000000000000";
//
//    if (client)
//    {
//        if (client->started())
//        {
//            client->send(_msg, false, _sub_user_uuid, _uuid_form, "message", objectName, msg_ref);
//        }
//    }
}

void ws_drv::reset_unread_messages(const std::string &user_sender, const std::string &uuid_form) {
//    try {
//
//        auto json = arcirk::bJson();
//        json.SetObject();
//        json.addMember("sender", user_sender);
//
//        send_command("reset_unread_messages", uuid_form, json.to_string());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

void ws_drv::get_channel_token(const std::string &user_sender, const std::string &uuid_form) {

}

void ws_drv::close(const variant_t& exit_base) {

//    if (client)
//    {
//        if (client->started())
//        {
//            client->close(std::get<bool>(exit_base));
//        }
//
//    }
}

void ws_drv::set_webdav_settings_on_client(const std::string &param) {
//    if(client)
//        client->set_webdav_settings_on_client(param);
}

void ws_drv::set_webdav_settings_on_server() {
//    try {
//        arcirk::bJson json{};
//        json.set_object();
//        json.addMember(arcirk::content_value("uuid_form", arcirk::nil_string_uuid()));
//        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavHost), get_webdav_host()));
//        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavUser), get_webdav_user()));
//        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavPwd), get_webdav_pwd()));
//        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavSSL), get_webdav_ssl()));
//
//        send_command("set_webdav_settings", nil_string_uuid(), json.to_string());
//
//    }catch (std::exception& e){
//        //message("error: " + std::string (e.what()));
//    }
}

std::string ws_drv::get_webdav_user() const {
//    if(client)
//        return client->get_webdav_user();
//    else
        return "";
}

std::string ws_drv::get_webdav_pwd() const {
//    if(client)
//        return client->get_webdav_pwd();
//    else
        return "";
}

std::string ws_drv::get_webdav_host() const {
//    if(client)
//        return client->get_webdav_host();
//    else
        return "";
}

bool ws_drv::get_webdav_ssl() {
//    if(client)
//        return client->get_webdav_ssl();
//    else
        return false;
}

std::string ws_drv::get_string_random_uuid() {
//    return arcirk::random_uuid();
    return "";
}

std::string ws_drv::get_parent_path() {
//    return arcirk::bConf::parent_path();
    return "";
}

void ws_drv::clientJoin(const std::string &resp) {

}

void ws_drv::getActiveUsers(const std::string &resp) {

}

void ws_drv::clientLeave(const std::string &resp) {

}

void ws_drv::getListUsers(const std::string &resp) {

}

void ws_drv::addGroupUsers(const std::string &resp) {

}

void ws_drv::editGroupUsers(const std::string &resp) {

}

void ws_drv::removeGroupUsers(const std::string &resp) {

}

void ws_drv::addUser(const std::string &resp) {

}

void ws_drv::updateUser(const std::string &resp) {

}

void ws_drv::deleteUser(const std::string &resp) {

}

void ws_drv::setUserParent(const std::string &resp) {

}

void ws_drv::save_conf(const variant_t &conf) {
//    std::string _conf(std::get<std::string>(conf));
//    if(!settings.parse(_conf))
//        return;
//    port = (int)settings[bConfFields::ServerPort].get_int();
//    host = settings[bConfFields::ServerHost].get_string();
//    admin_name = settings[bConfFields::User].get_string();
//    app_name = settings[bConfFields::AppName].get_string();
//    hash = settings[bConfFields::Hash].get_string();
//
//    if(app_name != "client_1C"){
//        app_name = "client_1C";
//        settings[bConfFields::AppName] = app_name;
//    }
//    settings.save();
}

void ws_drv::emit(const std::string& command, const std::string &resp) {
        this->ExternalEvent("WebSocketClient", command, resp);
}

void ws_drv::setWebDavSettingsToClient(const std::string &resp) {

//    set_webdav_settings_on_client(resp);
//
//    settings[bConfFields::WebDavHost] = client->get_webdav_host();
//    settings[bConfFields::WebDavUser] = client->get_webdav_user();
//    settings[bConfFields::WebDavPwd] = client->get_webdav_pwd();
//    settings[bConfFields::WebDavSSL] = client->get_webdav_ssl();
//
//    settings.save();
}

std::string ws_drv::crypt(const variant_t &source, const variant_t &key) {

//    std::string _source = std::get<std::string>(source);
//    std::string _key = std::get<std::string>(key);
//    return bConf::crypt(_source, _key);
    return "";
}

bool ws_drv::webdav_check() {

    std::map<std::string, std::string> options =
    {
            {"webdav_hostname", "https://arcirk.ru"},
            {"webdav_username", "webDavUser"},
            {"webdav_password", "@LbyFvj1"},
            {"webdav_root", "remote.php/dav/files/webDavUser"}
    };
    std::unique_ptr<WebDAV::Client> webdav_client{ new WebDAV::Client{ options } };
    bool check_connection = webdav_client->check();
    std::cout << "test connection with WebDAV drive is "
              << (check_connection ? "" : "not ")
              << "successful"<< std::endl;

    return check_connection;
//    return false;
//    std::map<std::string, std::string> options =
//    {
//            {"webdav_hostname", "https://arcirk.ru"},
//            {"webdav_username", "arcady"},
//            {"webdav_password", ""}
//    };
//
//    std::unique_ptr<WebDAV::Client> wd_client{ new WebDAV::Client{ options } };
//    auto remote_resources =
//    {
//            "existing_file.dat",
//            "not_existing_file.dat",
//            "existing_directory",
//            "existing_directory/",
//            "not_existing_directory",
//            "not_existing_directory/"
//    };
//
//    for (const auto& remote_resource : remote_resources)
//    {
//        bool is_existed = wd_client->check(remote_resource);
//        std::cout << "Resource: " << remote_resource
//                  << " is " << (is_existed ? "" : "not ") << "existed" << std::endl;
//    }

//    boost::dll::fs::path lib_path("C:/src/ws_solution/webdav_plugin/cmake-build-debug");             // argv[1] contains path to directory with our plugin library
//   boost::filesystem::path p = boost::filesystem::current_path();
//    boost::shared_ptr<bwebdav_api> plugin;            // variable to hold a pointer to plugin variable
//    std::cout << "Loading the plugin" << std::endl;
//
//    plugin = dll::import_symbol<bwebdav_api>(         // type of imported symbol is located between `<` and `>`
//            lib_path / "bwebdav",                     // path to the library and library name
//            "plugin",                                       // name of the symbol to import
//            dll::load_mode::append_decorations              // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
//    );
//    bool result = plugin->check_connection();
//    //std::cout << "plugin-check_connection() call:  " << plugin->check_connection() << std::endl;
//    return result;
//
  //  return true;
}

