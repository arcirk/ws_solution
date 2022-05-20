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
#include "sync_session.h"

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class  ws_client;

typedef std::function<void(std::string)> _callback_message;
typedef std::function<void(bool)> _callback_status;

//enum result_synch{
//    ok = 0,
//    error
//};

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

    ws_client * client;
    std::string _client_param;
    arcirk::bConf settings;

//    boost::mutex mtx;
//    boost::condition_variable cv;
//    bool _m_synch;
//    result_synch resultSynch;

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
    variant_t get_currentDate();
    variant_t get_current_date_in_seconds();
    variant_t get_tz_offset();
    [[nodiscard]] std::string get_app_uuid() const;
    [[nodiscard]] std::string get_user_uuid() const;

    void set_webdav_settings_on_client(const std::string& param);
    void set_webdav_settings_on_server();
    bool webdav_check();
    void get_webdav_settings(const variant_t& uuid_form);

    [[nodiscard]] std::string get_webdav_user() const;
    [[nodiscard]] std::string get_webdav_pwd() const;
    [[nodiscard]] std::string get_webdav_host() const;
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

    //bool synch_open(const variant_t &host, const variant_t &port);

    bool send_technical_information(const variant_t &host, const variant_t &port, const variant_t &usr,
                                    const variant_t &pwd, const variant_t &param);

    //bool synch_session_set_param(const variant_t &usr, const variant_t &pwd);
    //void synch_session_read();
    //void synch_write(const variant_t &msg);
    //void synch_session_close();
    //std::string synch_get_buffer();
    //bool synch_session_is_open();

//    void send_info_base_error_sync(const variant_t &host, const variant_t &port, const variant_t &usr, const variant_t &pwd,
//                               const variant_t &admin_ref, const variant_t &error_text);

};

#endif //SAMPLEADDIN_H
