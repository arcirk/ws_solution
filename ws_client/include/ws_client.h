//#include "net.h"

#include "global.h"
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <boost/asio/io_context.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

//#include <thread>

class session;

using std::placeholders::_1;
using std::placeholders::_2;

typedef std::function<void(std::string)> _callback_message;

class ws_client{

    std::unordered_set<session*> sessions_;
    std::mutex mutex_;

public:
    explicit
    ws_client(boost::asio::io_context &io_context, const std::string& client_param = "");

    void open(const char *host, const char *port);
    void open(const char *host, const char *port, const char *name);
    void open(const char *host, const char *port, const char *name, const char *uuid);
    void open(const char* host, const char* port, _callback_message& msg);
    void send(const std::string &message, bool is_cmd = true, const std::string& sub_user_uuid = "", const std::string& uuid_form = "", const std::string& command = "message");
//    void
//    to_channel(const std::string &message, const std::string& uuid_sub, const std::string& uuid_form);
    void on_connect(session * sess);
    void on_read(const std::string& message);
    void on_stop();
    void close();
    bool& started();
    boost::uuids::uuid& get_uuid();
    boost::uuids::uuid& get_user_uuid();
    std::string& get_name();
    void set_name(std::string name);
    std::string& get_app_name();
    void set_uuid(const std::string& uuid);
    void set_user_uuid(const std::string& uuid);
    void set_uuid();
    void set_user_uuid();
    std::string get_client_info();

    void error(const std::string &what, const std::string &err);

    static boost::uuids::uuid string_to_uuid(const std::string& uuid){return arc_json::string_to_uuid(uuid);};
    static std::string get_hash(const std::string& name,
                                const std::string& pwd){return arc_json::get_sha1(name + pwd);};
    void subscribe_to_channel(const std::string &uuid_channel, const std::string &uuid_form);
    void subscribe_exit_channel(const std::string &uuid_channel, const std::string &uuid_form);
    void set_session_param(const std::string &uuid, const std::string &name, const std::string &pwd, const std::string &app_name, const std::string &user_uuid);

    bool decode_message;
private:

    boost::asio::io_context &ioc;
    bool _started;
    boost::uuids::uuid uuid_{};
    std::string name_;
    _callback_message _callback_msg;

    std::string _client_param;

//    std::string _hash;
    std::string _app_name;
//    std::string _uuid_form;
    boost::uuids::uuid  _user_uuid{};

    static void console_log(const std::string logText){
        std::cout << logText << std::endl;
    }

    void send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param, session * sess);

    void set_param(ptree& pt);
};
