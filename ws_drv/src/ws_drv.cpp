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
//#include <thread>


#include "ws_drv.h"
//#include <boost/locale.hpp>
//#include <boost/lexical_cast.hpp>

//#include <mutex>

typedef std::function<void()> _start_func;


std::string ws_drv::extensionName() {
    return "WebSocketClient";
}

ws_drv::ws_drv() {

    client = nullptr;

    AddProperty(L"Version", L"Версия", [&]() {
        auto s = std::string(Version);
        return std::make_shared<variant_t>(std::move(s));
    });
    port = std::make_shared<variant_t>(8080);
    AddProperty(L"port", L"Порт", port);
    server = std::make_shared<variant_t>(std::move(std::string("127.0.0.1")));
    AddProperty(L"server", L"Сервер", server);
    log_path = std::make_shared<variant_t>(std::move(std::string("")));
    AddProperty(L"log_path", L"КаталогЛогФайлов", log_path);
    user_uuid = std::make_shared<variant_t>(std::move(std::string("")));
    client_uuid = std::make_shared<variant_t>(std::move(std::string("")));
    AddProperty(L"user_uuid", L"ИдентификаторПользователя", [&](){
        if (client){
            if (client->started())
                return std::make_shared<variant_t>(uuid_to_string(client->get_user_uuid()));
            else
                return user_uuid;
        } else
            return user_uuid;
    });

    AddProperty(L"client_uuid", L"ИдентификаторКлиента", [&](){
        if (client){
            if (client->started())
                return std::make_shared<variant_t>(uuid_to_string(client->get_uuid()));
            else
                return client_uuid;
        } else
            return client_uuid;
    });

    AddMethod(L"Open", L"Открыть", this, &ws_drv::open);
    AddMethod(L"Close", L"Закрыть", this, &ws_drv::close);
    AddMethod(L"Deliver", L"Сообщить", this, &ws_drv::send);
    AddMethod(L"Started", L"Запущен", this, &ws_drv::started);
    AddMethod(L"OpenAs", L"ОткрытьКак", this, &ws_drv::open_as);
    AddMethod(L"GetClientInfo", L"ПолучитьИнформациюОТекущемКлиенте", this, &ws_drv::get_client_info);
    AddMethod(L"CurrentName", L"ТекущееИмя", this, &ws_drv::get_current_name);
    AddMethod(L"CloseChannel", L"ОтключитсяОтКанала", this, &ws_drv::close_channel);
    AddMethod(L"JoinChannel", L"ПрисоединитсяККаналу", this, &ws_drv::join_channel);
    AddMethod(L"SendCommand", L"КомандаСерверу", this, &ws_drv::send_command);
    AddMethod(L"GetHash", L"ПолучитьХеш", this, &ws_drv::get_hash);
    AddMethod(L"CurrentDate", L"ТекущаяДата", this, &ws_drv::currentDate);
    AddMethod(L"CurrentDateSeconds", L"ТекущаяДатаВСекундах", this, &ws_drv::current_date_in_seconds);
    AddMethod(L"GetTZOffset", L"ПолучитьСмещениеВременнойЗоны", this, &ws_drv::get_tz_offset);
    AddMethod(L"GetMessages", L"ПолучитьСообщения", this, &ws_drv::get_messages);
    AddMethod(L"GetUserInfo", L"ПолучитьИнформациюОПользователе", this, &ws_drv::get_user_info);
    AddMethod(L"GetGroupList", L"ПолучитьСписокГрупп", this, &ws_drv::get_group_list);
    AddMethod(L"AddGroup", L"ДобавитьГруппу", this, &ws_drv::add_group);
    AddMethod(L"EditGroup", L"ИзменитьГруппу", this, &ws_drv::edit_group);
    AddMethod(L"RemoveGroup", L"УдалитьГруппу", this, &ws_drv::remove_group);
    AddMethod(L"GetUsers", L"ПолучитьПользователей", this, &ws_drv::get_users);
    AddMethod(L"SetParent", L"УстановитьГруппу", this, &ws_drv::set_parent);
    AddMethod(L"RemoveUser", L"УдалитьПользователя", this, &ws_drv::remove_user);

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

void ws_drv::ext_message(const std::string& msg) {

    //#ifdef _WINDOWS
    //    using namespace boost::locale::conv;
    //    std::string _msg = utf_to_utf<char>(msg);
    //    this->ExternalEvent("WebSocketAddIn", "message", _msg);
    //#else
    //this->ExternalEvent("WebSocketAddIn", "message", msg);
    //#endif // _WINDOWS

    this->ExternalEvent("WebSocketAddIn", "message", msg);
}

void ws_drv::start(){

    _callback_message callback = std::bind(&ws_drv::ext_message, this, std::placeholders::_1);

    std::string _port = std::to_string(std::get<int32_t>(*port));
    std::string _host = std::get<std::string>(*server);

    auto const host = _host.c_str();
    auto const port = _port.c_str();

    boost::asio::io_context ioc;

    close();

    client = new ws_client(ioc);
    client->open(host, port, callback);

}

void ws_drv::start_() {

    _callback_message callback = std::bind(&ws_drv::ext_message, this, std::placeholders::_1);

    std::string _port = std::to_string(std::get<int32_t>(*port));
    std::string _host = std::get<std::string>(*server);

    auto const host = _host.c_str();
    auto const port = _port.c_str();

    boost::asio::io_context ioc;

    close();

    std::string _param = std::get<std::string>(_client_param);

    client = new ws_client(ioc, _param);

    client->open(host, port, callback);

}

void ws_drv::close() {

    if (client)
    {
        if (client->started())
        {
            client->close();
        }

        delete client;
        client = nullptr;
    }

}

int ws_drv::open()
{
#ifdef _WINDOWS
    std::thread(std::bind(&ws_drv::start, this)).detach();
#else
    boost::thread(boost::bind(&ws_drv::start, this)).detach();
#endif

    return EXIT_SUCCESS;

}

int ws_drv::open_as(const variant_t &param) {

    _client_param = param;

#ifdef _WINDOWS
    std::thread(std::bind(&ws_drv::start_, this)).detach();
#else
    boost::thread(boost::bind(&ws_drv::start_, this)).detach();
#endif

    return EXIT_SUCCESS;

}

void ws_drv::send(const variant_t& msg, const variant_t& sub_user_uuid, const variant_t& uuid_form)
{
    std::string _msg = std::get<std::string>(msg);
    std::string _uuid_form = std::get<std::string>(uuid_form);
    std::string _sub_user_uuid = std::get<std::string>(sub_user_uuid);

    if (_msg.empty())
        return;
    if (_uuid_form.empty())
        _uuid_form = "00000000-0000-0000-0000-000000000000";
    if (_sub_user_uuid.empty())
        _sub_user_uuid = "00000000-0000-0000-0000-000000000000";

    if (client)
    {
        if (client->started())
        {
            client->send(_msg, false, _sub_user_uuid);
        }
    }
}

void ws_drv::send_command(const variant_t &cmd, const variant_t &uuid_form, const variant_t &param) {

    if (client){

        if (!started())
            return;

        std::string _cmd = std::get<std::string>(cmd);
        std::string _uuid_form = std::get<std::string>(uuid_form);
        std::string _param = std::get<std::string>(param);

        if (_cmd.empty())
            return;
        if (_uuid_form.empty())
            _uuid_form = "00000000-0000-0000-0000-000000000000";

        if (_param.empty())
            _param = "{\"command\":\"" + _cmd + "\"}";

        client->send(_param, true, "00000000-0000-0000-0000-000000000000", _uuid_form, _cmd);

    }

}

void ws_drv::send_command_(const std::string &cmd, const std::string &uuid_form, const std::string &param) {

    if (client){

        if (!started())
            return;

        std::string _cmd = cmd;
        std::string _uuid_form = uuid_form;
        std::string _param = param;

        if (_cmd.empty())
            return;
        if (_uuid_form.empty())
            _uuid_form = "00000000-0000-0000-0000-000000000000";

        if (_param.empty())
            _param = "{\"command\":\"" + _cmd + "\"}";

        client->send(_param, true, "00000000-0000-0000-0000-000000000000", _uuid_form, _cmd);
    }

}

void ws_drv::to_channel(const variant_t &msg, const variant_t &uuid_sub, const variant_t &uuid_form) {

    std::string _msg = std::get<std::string>(msg);
    std::string _uuid_form = std::get<std::string>(uuid_form);
    std::string _uuid_sub = std::get<std::string>(uuid_sub);

    if (_msg.empty())
        return;
    if (_uuid_form.empty())
        _uuid_form = "00000000-0000-0000-0000-000000000000";
    if (_uuid_sub.empty())
        _uuid_sub = "00000000-0000-0000-0000-000000000000";


}

bool ws_drv::started() {
    bool result = false;
    if (client){
        result = client->started();
    }
    return result;
}

variant_t ws_drv::get_client_info() {
    if (client){
        return client->get_client_info();

    }else
        return "";
}

std::string ws_drv::get_current_name() {
    if (client)
        return client->get_name();
    else
        return "unknown";
}

void ws_drv::join_channel(const variant_t &uuid_sub, const variant_t &uuid_form) {
    if (client){
        try {
            std::string uuid_channel(std::get<std::string>(uuid_sub));
            std::string _uuid_form(std::get<std::string>(uuid_form));

            client->subscribe_to_channel(uuid_channel.c_str(), _uuid_form.c_str());

        }catch (std::exception & e){
            set_log(std::string ("error: ") + e.what(), "/errors.log");
        }

    }
}

void ws_drv::close_channel(const variant_t &uuid_sub, const variant_t &uuid_form) {
    if (client){
        try {
            std::string uuid_channel(std::get<std::string>(uuid_sub));
            std::string _uuid_form(std::get<std::string>(uuid_form));

            client->subscribe_exit_channel(uuid_channel, _uuid_form);

        }catch (std::exception & e){
            set_log(std::string ("error: ") + e.what(), "/errors.log");
        }
    }
}

void ws_drv::set_log(const std::string &msg, const std::string &filename) {
    //    try {
    //        std::string _log_path = std::get<std::string>(*log_path);
    //        if (_log_path.empty())
    //            return;
    //        if (msg.empty())
    //            return;
    //
    ////        std::ofstream f;
    ////        f.open(_log_path + filename, std::ios::app);
    ////        f << msg << "\n";
    ////        f.close();
    //    }catch (std::exception & e){
    //        //
    //    }
}

std::string ws_drv::get_hash(const variant_t &usr, const variant_t &pwd) {

    std::string _usr(std::get<std::string>(usr));
    std::string _pwd(std::get<std::string>(pwd));

    return get_hash(_usr, _pwd);
}

variant_t ws_drv::currentDate() {
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

variant_t ws_drv::current_date_in_seconds() {
    return (int)current_date_seconds();
}

variant_t ws_drv::get_tz_offset() {
    return (int) tz_offset();
}

void ws_drv::get_messages(const variant_t &uuid_sub, const variant_t &start_date, const variant_t &end_date, const variant_t &limit, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_sub = std::get<std::string>(uuid_sub);

        tm _start_date = std::get<tm>(start_date);
        std::chrono::system_clock::time_point tp_start = std::chrono::system_clock::from_time_t(mktime(&_start_date));
        long int int_start_date = std::chrono::duration_cast<std::chrono::seconds>(tp_start.time_since_epoch()).count();

        tm _end_date = std::get<tm>(end_date);
        std::chrono::system_clock::time_point tp_end = std::chrono::system_clock::from_time_t(mktime(&_end_date));
        long int int_end_date = std::chrono::duration_cast<std::chrono::seconds>(tp_end.time_since_epoch()).count();

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("recipient", _uuid_sub);
        pt.add("start_date", int_start_date);
        pt.add("end_date", int_end_date);
        pt.add("limit", std::get<int>(limit));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("get_messages", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }

}

void ws_drv::get_user_info(const variant_t &user_uuid, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_sub = std::get<std::string>(user_uuid);
        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("user_uuid", _uuid_sub);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("get_user_info", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void ws_drv::get_group_list(const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        send_command_("get_group_list", _uuid_form, "");

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void
ws_drv::add_group(const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("name", std::get<std::string>(name));
        pt.add("presentation", std::get<std::string>(presentation));
        pt.add("parent", std::get<std::string>(uuid_parent));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("add_group", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void ws_drv::edit_group(const variant_t &uuid_group, const variant_t &name, const variant_t &presentation,
                        const variant_t &uuid_parent, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("name", std::get<std::string>(name));
        pt.add("presentation", std::get<std::string>(presentation));
        pt.add("parent", std::get<std::string>(uuid_parent));
        pt.add("ref", std::get<std::string>(uuid_group));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("edit_group", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void ws_drv::remove_group(const variant_t &uuid_group, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("ref", std::get<std::string>(uuid_group));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("remove_group", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }

}

void ws_drv::get_users(const variant_t &uuid_group, const variant_t &uuid_form) {
    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("channel", std::get<std::string>(uuid_group));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("get_users", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void ws_drv::set_parent(const variant_t &user_uuid, const variant_t &uuid_group, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("parent", std::get<std::string>(uuid_group));
        pt.add("user", std::get<std::string>(user_uuid));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("set_parent", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}

void ws_drv::remove_user(const variant_t &user_uuid, const variant_t &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = std::get<std::string>(uuid_form);

        pt.add("ref", std::get<std::string>(user_uuid));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command_("remove_user", _uuid_form, _ss.str());

    }catch (std::exception& e){
        message("error: " + std::string (e.what()));
    }
}
