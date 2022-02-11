#ifndef IWS_CLIENT_H
#define IWS_CLIENT_H

#include <iostream>
#include <string>
#include <functional>

#include "iws_client_global.h"

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class  ws_client;

//typedef std::function<void(std::string)> _callback_message;
typedef std::function<void(std::string)> _callback_message;
typedef std::function<void(bool)> _callback_status;

enum result_synch{
    ok = 0,
    error
};

class  WS_CLIENT_EXPORT IClient{

public:
    std::string app_uuid;
    std::string user_uuid;
    //std::string client_uuid;
    std::string host;
    int port;
    std::string admin_name;
    std::string app_name;
    std::string hash;

    explicit
    IClient();
    explicit
    IClient(const std::string& _host, const int& _port, _callback_message& callback);
    explicit
    IClient(const std::string& _host, const int& _port, _callback_message& callback, _callback_status& _status_changed_fun);
    ~IClient(){
        close();
    };

    void ext_message(const std::string& msg);

    void close();
    void open(bool new_thread = true);
    bool synch_open();

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

    [[nodiscard]] std::string get_app_uuid() const;
    [[nodiscard]] std::string get_user_uuid() const;

    static tm currentDate();
    static long int current_date_in_seconds();
    long int get_tz_offset();

    static std::string base64_encode(const std::string &s);
    static std::string base64_decode(const std::string &s);
    static std::string nil_string_uuid();
    static std::string random_uuid();
    static std::string get_hash(const std::string &usr, const std::string &pwd);
    static long int current_date_seconds();
    static long int add_day(const long int selDate, const int dayCount);

    void set_webdav_settings_on_client(const std::string& param);
    void set_webdav_settings_on_server();

    [[nodiscard]] std::string get_webdav_user() const;
    [[nodiscard]] std::string get_webdav_pwd() const;
    [[nodiscard]] std::string get_webdav_host() const;
    bool get_webdav_ssl();

    static std::string get_string_random_uuid();
    static std::string get_parent_path();

    //synch client

    //bool synch_session_set_param(const std::string  &usr, const std::string  &pwd);
    void synch_session_read();
    void synch_session_write(const std::string  &msg);
    //void synch_session_close();
    std::string synch_session_get_buffer();
    //bool synch_session_is_open();

private:
    ws_client * client;
    std::string _client_param;
    _callback_message callback_msg;
    _callback_status _status_changed;

    boost::mutex mtx;
    boost::condition_variable cv;
    bool _m_synch;
    result_synch resultSynch;

    void start();

};

#endif // IWS_CLIENT_H
