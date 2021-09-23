#include "../include/iws_client.h"

#ifdef _WINDOWS
#include <iostream>
#include <thread>
#else
#include <boost/thread/thread.hpp>
#endif // _WINDOWS

IClient::IClient(const std::string& _host, const int& _port, _callback_message& callback)
{
    host = _host;
    port = _port;
    callback_msg = callback;
    client = nullptr;
}

void IClient::send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param) {

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

}

void IClient::start() {

   //_callback_message callback = std::bind(&IClient::ext_message, this, std::placeholders::_1);

    boost::asio::io_context ioc;

    close();

    client = new ws_client(ioc, _client_param);

    //client->open(host.c_str(), std::to_string(port).c_str(), callback);
    client->open(host.c_str(), std::to_string(port).c_str(), callback_msg);

    delete client;
    client = nullptr;

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
    return arc_json::current_date_seconds();
}

long int IClient::get_tz_offset() {
    return arc_json::tz_offset();
}

void IClient::get_messages(const std::string &uuid_sub, long int &start_date, const long int &end_date, int &limit, const std::string &uuid_form) {

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

void IClient::get_user_info(const std::string &user_uuid, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("user_uuid", user_uuid);

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

void IClient::set_parent(const std::string &user_uuid, const std::string &uuid_group, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("parent", uuid_group);
        pt.add("user", user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("set_parent", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::remove_user(const std::string &user_uuid, const std::string &uuid_form) {

    boost::property_tree::ptree pt;

    try {

        pt.add("ref", user_uuid);

        std::stringstream _ss;
        boost::property_tree::json_parser::write_json(_ss, pt);

        send_command("remove_user", uuid_form, _ss.str());

    }catch (std::exception& e){
        //message("error: " + std::string (e.what()));
    }
}

void IClient::open(bool new_thread){

    app_uuid = arc_json::random_uuid();
    user_uuid = arc_json::random_uuid();
    client_uuid = arc_json::random_uuid();\

    boost::property_tree::ptree pt;

    pt.add("uuid", app_uuid);
    pt.add("name", admin_name);
    pt.add("hash", hash);
    pt.add("app_name", "admin_console");
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
    return arc_json::get_hash(usr, pwd);
}