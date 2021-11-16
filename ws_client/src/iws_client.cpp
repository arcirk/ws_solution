#include "../include/iws_client.h"

#ifdef _WINDOWS
#include <iostream>
#include <thread>
#else
#include <boost/thread/thread.hpp>
#endif // _WINDOWS

//using namespace arcirk;

IClient::IClient(const std::string& _host, const int& _port, _callback_message& callback)
{
    host = _host;
    port = _port;
    callback_msg = callback;
    client = nullptr;
    app_name = "admin_console";
    //client_uuid = arc_json::random_uuid();
    user_uuid = nil_string_uuid();
}
IClient::IClient(const std::string &_host, const int &_port, _callback_message &callback,
                 _callback_status &_status_changed_fun) {
    host = _host;
    port = _port;
    callback_msg = callback;
    client = nullptr;
    app_name = "admin_console";
    //client_uuid = arc_json::random_uuid();
    user_uuid = nil_string_uuid();
    _status_changed = _status_changed_fun;
}

void IClient::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param) {

    if (client){

        if (!started())
            return;

        const std::string& _cmd = cmd;
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

void IClient::ext_message(const std::string& msg) {
    //std::cout << msg << std::endl;
    if(callback_msg){
        callback_msg(msg);
    }
}

void IClient::close() {

    if (client)
    {
        if (client->started())
        {
            client->close();
        }

        delete client;
        client = nullptr;
    }
//
//    if(_status_changed){
//        _status_changed(false);
//    }

}

void IClient::start() {

   //_callback_message callback = std::bind(&IClient::ext_message, this, std::placeholders::_1);

    boost::asio::io_context ioc;

    close();

    client = new ws_client(ioc, _client_param);
    //client->set_status_callback(callback_status_changed);
    //client->open(host.c_str(), std::to_string(port).c_str(), callback);
    client->open(host.c_str(), std::to_string(port).c_str(), callback_msg, _status_changed);
    //client->set_status_callback(_callback_status_changed);
    delete client;
    client = nullptr;

    std::cout << "exit client thread" << std::endl;
}

bool IClient::started() {
    bool result = false;

//    try {
        if (client){
            result = client->started();
        }
//    }  catch(std::exception&) {
//        return false;
//    }

    return result;
}

tm IClient::currentDate() {
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

long int IClient::current_date_in_seconds() {
    return current_date_seconds();
}

long int IClient::get_tz_offset() {
    return tz_offset();
}

void IClient::get_messages(const std::string &uuid_sub, const long int &start_date, const long int &end_date, int &limit, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("recipient", uuid_sub);
        pt.add("start_date", (int)start_date);
        pt.add("end_date", (int)end_date);
        pt.add("limit", limit);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_messages", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }

}

void IClient::get_user_info(const std::string &_user_uuid, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("uuid_user", _user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_user_info", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::get_group_list(const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        send_command("get_group_list", uuid_form, "");

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::add_group(const std::string &name, const std::string &presentation, const std::string &uuid_parent, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("name", name);
        pt.add("presentation", presentation);
        pt.add("parent", uuid_parent);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("add_group", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::edit_group(const std::string &uuid_group, const std::string &name, const std::string &presentation,
                        const std::string &uuid_parent, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("name", name);
        pt.add("presentation", presentation);
        pt.add("parent", uuid_parent);
        pt.add("ref", uuid_group);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("edit_group", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::remove_group(const std::string &uuid_group, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {
        pt.add("ref", uuid_group);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("remove_group", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }

}

void IClient::get_users(const std::string &uuid_group, const std::string &uuid_form) {
    boost::property_tree::ptree pt;

    try {

        pt.add("channel", uuid_group);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_users", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::set_parent(const std::string &_user_uuid, const std::string &uuid_group, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("parent", uuid_group);
        pt.add("user", _user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("set_parent", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::remove_user(const std::string &_user_uuid, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("ref", _user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("remove_user", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::kill_session(const std::string &_user_uuid, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("ref", _user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("kill_session", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::open(bool new_thread){

    app_uuid = random_uuid();
    user_uuid = random_uuid();
    //client_uuid = arc_json::random_uuid();

    boost::property_tree::ptree pt;

    pt.add("uuid", app_uuid);
    pt.add("name", admin_name);
    pt.add("hash", hash);
    pt.add("app_name", app_name);
    pt.add("user_uuid", user_uuid);

    std::stringstream _ss;
    boost::property_tree::json_parser::write_json(_ss, pt);

    _client_param = _ss.str();

    if (new_thread){
    #ifdef _WINDOWS
        std::thread(std::bind(&IClient::start, this)).detach();
    #else
        boost::thread(boost::bind(&IClient::start, this)).detach();
    #endif
    }else
        start();

}

std::string IClient::get_hash(const std::string &usr, const std::string &pwd) {
    return arcirk::get_hash(usr, pwd);
}

std::string IClient::get_app_uuid() const {
    if (client->started())
        return arcirk::uuid_to_string(client->get_uuid());
    else
        return app_uuid;
}

void IClient::set_app_name(const std::string &session_uuid, const std::string &new_app_name) {

    boost::property_tree::ptree pt;

    try {

        pt.add("uuid_set", session_uuid);
        pt.add("new_app_name", new_app_name);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("set_app_name", nil_string_uuid(), _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::get_users_catalog(const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = uuid_form;

        if (uuid_form.empty()){
            _uuid_form = nil_string_uuid();
        }
        pt.add("uuid_form", _uuid_form);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_users_catalog", nil_string_uuid(), _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

std::string IClient::get_user_uuid() const {

    return uuid_to_string(client->get_user_uuid());
}

void IClient::get_user_cache(const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = uuid_form;

        if (uuid_form.empty()){
            _uuid_form = nil_string_uuid();
        }
        pt.add("uuid_form", _uuid_form);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_user_cache", nil_string_uuid(), _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::set_user_cache(const std::string &cache, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        std::string _uuid_form = uuid_form;

        if (uuid_form.empty()){
            _uuid_form = nil_string_uuid();
        }
        pt.add("uuid_form", _uuid_form);
        pt.add("cache", arcirk::base64_encode(cache));

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("set_user_cache", nil_string_uuid(), _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::send(const std::string &msg, const std::string &sub_user_uuid, const std::string &uuid_form) {

    std::string _msg = msg;
    std::string _uuid_form = uuid_form;
    std::string _sub_user_uuid = sub_user_uuid;

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
            client->send(_msg, false, _sub_user_uuid, _uuid_form);
        }
    }
}

//bool IClient::is_open() {
//    if(client){
//        return client->is_open();
//    }
//    return false;
//}

