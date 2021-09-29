//
// Created by arcady on 05.07.2021.
//

#ifndef UNTITLED1_SHARED_STATE_HPP
#define UNTITLED1_SHARED_STATE_HPP

#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>
#include <set>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators e

#include "subscriber.h"
#include "../include/base.h"

#include <arc_json.h>

//#include <ws_client_console.h>

// Forward declaration
class websocket_session;

typedef std::function<bool(boost::uuids::uuid &uuid, arc_json::ws_json*, arc_json::ws_message*, std::string&, std::string&)> cmd_func;

// Represents the shared server state
class shared_state
{
    std::string const doc_root_;

    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;

    // Keep a list of all the connected clients
    //std::unordered_set<websocket_session*> sessions_;
    std::map<boost::uuids::uuid, websocket_session*> sessions_; //сессии
    std::map<boost::uuids::uuid, std::vector<websocket_session*>> user_sessions; //все сессии пользователя
    std::map<std::string, std::vector<boost::uuids::uuid>> channels; //каналы



public:
    explicit
    shared_state(std::string doc_root);
    ~shared_state(){
        sqlite3Db->close();
        delete sqlite3Db;
    };

    std::string const&
    doc_root() const noexcept
    {
        return doc_root_;
    }

    void join  (websocket_session* session);
    void leave (websocket_session* session);
    void send(const std::string& message, std::string command = "message", bool to_base64 = true);
    void deliver(const std::string& message, websocket_session* session);
    void on_start();



private:
    arc_sqlite::sqlite3_db* sqlite3Db;

    static bool is_cmd(const std::string& msg);
    static bool is_msg(const std::string& msg);
    std::string run_cmd(const std::string &cmd, boost::uuids::uuid &uuid, std::string &command, std::string &uuid_form,
                        std::string &res);
    void send_message(const std::string &message, boost::uuids::uuid &recipient, websocket_session *current_sess);
    static bool is_valid_message(const std::string &message, boost::uuids::uuid &uuid_recipient, std::string &base64,
                          std::string &err);

    bool get_active_users(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool get_db_users(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool send_all_message(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool set_client_param(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool subscribe_to_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool subscribe_exit_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool add_new_user(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool update_user(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool get_messages(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool get_user_info(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool get_group_list(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool add_group(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool edit_group(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool remove_group(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool set_parent(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool remove_user(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);
    bool kill_session(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result);


    websocket_session* get_session(boost::uuids::uuid &uuid);
    std::vector<websocket_session *> get_sessions(boost::uuids::uuid &user_uuid);

    void send_private_message(const std::string &message, boost::uuids::uuid &recipient,
                              boost::uuids::uuid &sender);

    void _add_new_user(const std::string &usr, const std::string &pwd, const std::string &role,
                       const std::string &uuid, const std::string &perf, const std::string &parent = (std::string &) "", std::string& error = (std::string &) "", bool pwd_is_hash = false);






    bool is_valid_param_count(const std::string &command, unsigned int params);
    static bool is_valid_command_name(const std::string &command);
    cmd_func get_cmd_func(const std::string& command);

    void get_group_hierarchy_keys(const std::string &current_uuid, std::set<int> &vec_uuid, std::string &err);

    void remove_group_hierarchy(const std::string &current_uuid, std::string &err);
};


#endif //UNTITLED1_SHARED_STATE_HPP
