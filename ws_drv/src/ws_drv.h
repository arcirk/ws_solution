/*
 *  Modern Native AddIn
 *  Copyright (C) 2018  Infactum
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will аbe useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SAMPLEADDIN_H
#define SAMPLEADDIN_H

#include "Component.h"

#ifdef _WINDOWS
//    #include <net.h>
#endif // _WINDOWS

#include <wdclient.hpp>
#include <arcirk.h>
#include <synch_session.h>

class  ws_client;

typedef std::function<void(std::string)> _callback_message;
typedef std::function<void(bool)> _callback_status;

class ws_drv final : public Component {
public:
    const char *Version = u8"1.0.0";
    std::string app_uuid;
    std::string user_uuid;
    std::string host;
    int port;
    std::string admin_name;
    std::string app_name;
    std::string hash;

    ws_drv();

    void ext_message(const std::string& msg);
    void status_changed(bool status);

    void close(const variant_t& exit_base = false);

    void open_as(const variant_t &_host, const variant_t &_port, const variant_t &_user, const variant_t &_pwd,
                 variant_t save_conf);

private:
    std::string extensionName() override;
    void processServeResponse(const std::string &jsonResp);
    void _open(bool new_thread = true);
    void open();
    std::string  get_client_conf();
    void get_messages(const std::string &uuid_sub, const long int &start_date, const long int &end_date, int &limit, const std::string &uuid_form);
    void get_user_info(const std::string &_user_uuid, const std::string &uuid_form);
    void get_group_list(const std::string &uuid_form);
    void add_group(const std::string &name, const std::string &presentation, const std::string &uuid_parent, const std::string &uuid_form);
    void edit_group(const std::string &uuid_group, const std::string &name, const std::string &presentation, const std::string &uuid_parent, const std::string &uuid_form);
    void remove_group(const std::string &uuid_group, const std::string &uuid_form);
    void get_users(const std::string &uuid_group, const std::string &uuid_form);
    void set_parent(const std::string &_user_uuid, const std::string &uuid_group, const std::string &uuid_form);
    void remove_user(const std::string &_user_uuid, const std::string &uuid_form);
    void kill_session(const std::string &_user_uuid, const std::string &uuid_form);
    void send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param);
    bool started();
    void set_app_name(const std::string &session_uuid, const std::string& new_app_name);
    void get_users_catalog(const std::string &uuid_form);
    void get_user_cache(const std::string &uuid_form);
    void set_user_cache(const std::string &cache, const std::string &uuid_form);
    void get_user_status(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param);
    void get_user_data(const std::string &_user_uuid, const std::string &uuid_form, const std::string &param);
    void send(const std::string& msg, const std::string& sub_user_uuid, const std::string& uuid_form, const std::string& objectName = "", const std::string& msg_ref = "");
    void reset_unread_messages(const std::string &user_sender, const std::string &uuid_form);
    void get_channel_token(const std::string &user_sender, const std::string &uuid_form);
    void message(const variant_t &msg);
    static variant_t get_currentDate();
    static variant_t get_current_date_in_seconds();
    static variant_t get_tz_offset();
    [[nodiscard]] std::string get_app_uuid() const;
    [[nodiscard]] std::string get_user_uuid() const;

    void set_webdav_settings_on_client(const std::string& param);
    void set_webdav_settings_on_server();
    bool webdav_check();
    void get_webdav_settings(const variant_t& uuid_form);

    std::string get_webdav_user() const;
    std::string get_webdav_pwd() const;
    std::string get_webdav_host() const;
    bool get_webdav_ssl();

    static std::string get_string_random_uuid();
    static std::string get_parent_path();

    std::string crypt(const variant_t &source, const variant_t& key);

    //сообщения
    void displayError(const std::string& what, const std::string& err);
    void connectionSuccess();
    void closeConnection();
    void connectedStatusChanged(bool status);
    void userInfo(const std::string& uuid);
    void clientJoin(const std::string& resp);
    void getActiveUsers(const std::string& resp);
    void clientLeave(const std::string& resp);
    void setMessages(const std::string& resp);
    void messageReceived(const std::string& msg, const std::string& uuid, const std::string& recipient, const std::string& recipientName, const std::string& uuid_form);
    void requestUserStatus(const std::string& resp);
    void unreadMessages(const std::string& resp);
    void getGroupList(const std::string& resp);
    void getListUsers(const std::string& resp);
    void addGroupUsers(const std::string& resp);
    void editGroupUsers(const std::string& resp);
    void removeGroupUsers(const std::string& resp);

    void addUser(const std::string& resp);
    void updateUser(const std::string& resp);
    void deleteUser(const std::string& resp);
    void setUserParent(const std::string& resp);

    void start();

    std::string get_hash(const variant_t &usr, const variant_t &pwd);

    void save_conf(const variant_t &conf);

    void emit(const std::string& command, const std::string &resp);

    void setWebDavSettingsToClient(const std::string &resp);

    bool synch_session_open(const variant_t &host, const variant_t &port);
    bool synch_session_set_param(const variant_t &usr, const variant_t &pwd);
    void synch_session_read();
    void synch_session_write(const variant_t &msg);
    void synch_session_close();
    std::string synch_session_get_buffer();
    bool synch_session_is_open();

//
//    int open();
//
//    int open_as(const variant_t &param);
//
//    //int open_as(const std::string& param);
//
//    void close();
//
//    void send(const variant_t& msg, const variant_t& sub_user_uuid, const variant_t& uuid_form);

    //void start();

//    void start(const std::string &uuid, const std::string &name, const std::string &pwd,
//               const std::string &s_user_uuid);

   // void start_();

   // bool started();

//    variant_t get_client_info();
//
//    std::string get_current_name();

//    void join_channel(const variant_t &uuid, const variant_t &uuid_form);
//
//    void close_channel(const variant_t &uuid, const variant_t &uuid_form);

//    void set_log(const std::string &msg, const std::string &filename);
//
////    void to_channel(const variant_t& msg, const variant_t& uuid_sub, const variant_t& uuid_form);
//
//    //Команда серверу
//    void send_command(const variant_t& cmd, const variant_t& uuid_form, const variant_t& param);
//    //void send_command_(const std::string &cmd, const std::string &uuid_form, const std::string &param);
//
//    std::string get_hash(const variant_t &usr, const variant_t &pwd);
//    variant_t currentDate();
//    variant_t current_date_in_seconds();
//    variant_t get_tz_offset();
//    void get_messages(const variant_t &uuid_sub, const variant_t &start_date, const variant_t &end_date, const variant_t &limit, const variant_t &uuid_form);
//    void get_user_info(const variant_t &user_uuid, const variant_t &uuid_form);
//    void get_group_list(const variant_t &uuid_form);
//    void add_group(const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form);
//    void edit_group(const variant_t &uuid_group, const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form);
//    void remove_group(const variant_t &uuid_group, const variant_t &uuid_form);
//    void get_users(const variant_t &uuid_group, const variant_t &uuid_form);
//    void set_parent(const variant_t &user_uuid, const variant_t &uuid_group, const variant_t &uuid_form);
//    void remove_user(const variant_t &user_uuid, const variant_t &uuid_form);



    ws_client * client;
    std::string _client_param;
    arcirk::bConf settings;

    synch_session synchSession;

//    _callback_message callback_msg;
//    _callback_status _status_changed;


};

#endif //SAMPLEADDIN_H
