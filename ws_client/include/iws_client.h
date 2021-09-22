#ifndef IWS_CLIENT_H
#define IWS_CLIENT_H

//#define __IWS_CLIENT_ __declspec(dllexport)

#include <iostream>
#include <string>

#ifdef _WINDOWS
    #include "net.h"
    #include "ws_client.h"
#else
    #include "ws_client.h"
#endif // _WINDOWS

//typedef std::function<void(std::string)> _callback_message;


//__IWS_CLIENT_

class  IClient{

public:
    std::string app_uuid;
    std::string user_uuid;
    std::string client_uuid;
    std::string host;
    int port;
    std::string admin_name;
    std::string app_name;
    std::string hash;

    explicit
    IClient(const std::string& _host, const int& _port, _callback_message& callback);
    ~IClient(){
        close();
    };

    void ext_message(const std::string& msg);

    void close();
    void open();
    std::string get_client_info();
    std::string get_current_name();
    static std::string get_hash(const std::string &usr, const std::string &pwd);

    void get_messages(const std::string &uuid_sub, long int &start_date, const long int &end_date, int &limit, const std::string &uuid_form);
    void get_user_info(const std::string &user_uuid, const std::string &uuid_form);
    void get_group_list(const std::string &uuid_form);
    void add_group(const std::string &name, const std::string &presentation, const std::string &uuid_parent, const std::string &uuid_form);
    void edit_group(const std::string &uuid_group, const std::string &name, const std::string &presentation, const std::string &uuid_parent, const std::string &uuid_form);
    void remove_group(const std::string &uuid_group, const std::string &uuid_form);
    void get_users(const std::string &uuid_group, const std::string &uuid_form);
    void set_parent(const std::string &user_uuid, const std::string &uuid_group, const std::string &uuid_form);
    void remove_user(const std::string &user_uuid, const std::string &uuid_form);
    void send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param);
    bool started();

    tm currentDate();
    long int current_date_in_seconds();
    long int get_tz_offset();

private:

    //std::string log_path;

    ws_client * client;
    std::string _client_param;
    _callback_message callback_msg;

    int open_as(const std::string &param);
    void send(const std::string& msg, const std::string& sub_user_uuid, const std::string& uuid_form);
    void start();

};

#endif // IWS_CLIENT_H