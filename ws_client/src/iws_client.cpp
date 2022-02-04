#include "../include/iws_client.h"
#ifdef _WINDOWS
    #pragma warning(disable:4061)
    #pragma warning(disable:4001)
    #include  <SDKDDKVer.h>
    #include "../include/net.h"
    #include "../include/ws_client.h"
#else
#include "../include/ws_client.h"
#endif // _WINDOWS

#ifdef _WINDOWS
#include <iostream>
#include <thread>
#else
#include <boost/thread/thread.hpp>
#endif // _WINDOWS

IClient::IClient()
{
    host = "localhost";
    port = 8080;
    callback_msg = nullptr;
    client = nullptr;
    app_name = "admin_console";
    user_uuid = nil_string_uuid();
    _m_synch = false;
}

IClient::IClient(const std::string& _host, const int& _port, _callback_message& callback)
{
    host = _host;
    port = _port;
    callback_msg = callback;
    client = nullptr;
    app_name = "admin_console";
    user_uuid = nil_string_uuid();
    _m_synch = false;
}
IClient::IClient(const std::string &_host, const int &_port, _callback_message &callback,
                 _callback_status &_status_changed_fun) {
    host = _host;
    port = _port;
    callback_msg = callback;
    client = nullptr;
    app_name = "admin_console";
    user_uuid = nil_string_uuid();
    _status_changed = _status_changed_fun;
    _m_synch = false;
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

        //delete client;
        //client = nullptr;
    }


}

void IClient::start() {

    boost::asio::io_context ioc;

    close();

    client = new ws_client(ioc, _client_param);

    try {
        client->open(host.c_str(), std::to_string(port).c_str(), callback_msg, _status_changed);
    }
    catch (std::exception& e){
        std::cerr << "IClient::start::exception: " << e.what() <<std::endl;
    }

    std::cout << "IClient::start: exit client thread" << std::endl;

    if(client){
        delete client;
        client = nullptr;
    }

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

    }catch (std::exception&){
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

    }catch (std::exception&){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::get_group_list(const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        send_command("get_group_list", uuid_form, "");

    }catch (std::exception&){
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

    }catch (std::exception&){
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

    }catch (std::exception&){
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

    }catch (std::exception&){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::open(bool new_thread){

    app_uuid = random_uuid();
    user_uuid = random_uuid();

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
    if (client)
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

    }catch (std::exception&){
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

    if(client)
        return uuid_to_string(client->get_user_uuid());
    else
        return IClient::nil_string_uuid();
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

    }catch (std::exception&){
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

void IClient::send(const std::string &msg, const std::string &sub_user_uuid, const std::string &uuid_form, const std::string& objectName, const std::string& msg_ref) {

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
            client->send(_msg, false, _sub_user_uuid, _uuid_form, "message", objectName, msg_ref);
        }
    }
}

void IClient::get_user_status(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param) {

    try {
        auto json = arcirk::bJson();
        if(param.empty()){
            json.SetObject();
            json.addMember("uuid_user", _user_uuid);
        }
        else{
            json.parse(param);
            if (json.is_parse()){
                bVariant var;
                if (!json.getMember("uuid_user", var))
                    json.addMember("uuid_user", _user_uuid);
            }
        }

        send_command("get_user_status", uuid_form, json.to_string());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}


void IClient::get_user_data(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param) {

    try {
        auto json = arcirk::bJson();
        if(param.empty()){
            json.SetObject();
            json.addMember("uuid_user", _user_uuid);
        }
        else{
            json.parse(param);
            if (json.is_parse()){
                bVariant var;
                if (!json.getMember("uuid_user", var))
                    json.addMember("uuid_user", _user_uuid);
            }
        }

        send_command("get_user_data", uuid_form, json.to_string());

    }catch (std::exception&){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::reset_unread_messages(const std::string &user_sender, const std::string &uuid_form) {
    try {

        auto json = arcirk::bJson();
        json.SetObject();
        json.addMember("sender", user_sender);

        send_command("reset_unread_messages", uuid_form, json.to_string());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

std::string IClient::base64_encode(const std::string &s) {
    return  arcirk::base64_encode(s);
}

std::string IClient::base64_decode(const std::string &s) {
    return arcirk::base64_decode(s);
}

std::string IClient::nil_string_uuid() {
    return arcirk::nil_string_uuid();
}

std::string IClient::random_uuid() {
    return arcirk::random_uuid();
}

long int IClient::current_date_seconds() {
    return arcirk::current_date_seconds();
}

long int IClient::add_day(const long selDate, const int dayCount) {
    return arcirk::add_day(selDate, dayCount);
}

void IClient::set_webdav_settings_on_client(const std::string& param) {
    if(client)
        client->set_webdav_settings_on_client(param);

}

std::string IClient::get_webdav_user() const {
    if(client)
        return client->get_webdav_user();
    else
        return "";
}

std::string IClient::get_webdav_pwd() const {
    if(client)
        return client->get_webdav_pwd();
    else
        return "";
}

std::string IClient::get_webdav_host() const {
    if(client)
        return client->get_webdav_host();
    else
        return "";
}
bool IClient::get_webdav_ssl() {
    if(client)
        return client->get_webdav_ssl();
    else
        return false;
}

void IClient::set_webdav_settings_on_server() {

    //ptree не понимает тип bool, очень жаль
    //boost::property_tree::ptree pt;

    try {
//        pt.add("uuid_form", arcirk::nil_string_uuid());
//        pt.add(bConf::get_field_alias(bConfFields::WebDavHost), get_webdav_host());
//        pt.add(bConf::get_field_alias(bConfFields::WebDavUser), get_webdav_user());
//        pt.add(bConf::get_field_alias(bConfFields::WebDavPwd), get_webdav_pwd());
//        bool val = get_webdav_ssl();
//        pt.add(bConf::get_field_alias(bConfFields::WebDavSSL), val);
//
//        std::stringstream _ss;
//        boost::property_tree::json_parser::write_json(_ss, pt);


        arcirk::bJson json{};
        json.set_object();
        json.addMember(arcirk::content_value("uuid_form", arcirk::nil_string_uuid()));
        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavHost), get_webdav_host()));
        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavUser), get_webdav_user()));
        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavPwd), get_webdav_pwd()));
        json.addMember(arcirk::content_value(bConf::get_field_alias(bConfFields::WebDavSSL), get_webdav_ssl()));

        send_command("set_webdav_settings", nil_string_uuid(), json.to_string());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }

}

void IClient::get_channel_token(const std::string &user_sender, const std::string &uuid_form) {
    boost::property_tree::ptree pt;

    try {
        pt.add("uuid_form", uuid_form);
        pt.add("user_uuid", get_user_uuid());
        pt.add("recipient_uuid", user_sender);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("get_channel_token", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

std::string IClient::get_string_random_uuid() {
    return arcirk::random_uuid();
}

std::string IClient::get_parent_path() {
    return arcirk::bConf::parent_path();
}

bool IClient::synch_session_open(const std::string &host, const std::string &port) {

    if (client)
        return false;

    boost::asio::io_context ioc;
    client = new ws_client(ioc);

    bool result = client->synch_open(host.c_str(), port.c_str());

    if (result)
        _m_synch = true;

    return result;

}

bool IClient::synch_session_set_param(const std::string &usr, const std::string &pwd) {
    if (!_m_synch)
        return false;
    bool result = client->synch_set_param(usr, pwd);
    return result;
}

void IClient::synch_session_read() {
    if (!_m_synch)
        return;
    client->synch_read();
}

void IClient::synch_session_write(const std::string &msg) {
    if (!_m_synch)
        return;
    client->synch_write(msg);
}

void IClient::synch_session_close() {
    if (!_m_synch)
        return;
    client->synch_close();
    _m_synch = false;
}

std::string IClient::synch_session_get_buffer() {
    if (!_m_synch)
        return {};
    return client->synch_get_buffer();
}

bool IClient::synch_session_is_open() {
    if (!_m_synch)
        return false;
    return client->synch_is_open();
}
