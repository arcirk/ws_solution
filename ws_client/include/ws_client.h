//#include "net.h"

#include "../../arcirk/include/arcirk.h"
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <boost/asio/io_context.hpp>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

//#include <thread>

class session;

using std::placeholders::_1;
using std::placeholders::_2;

typedef std::function<void(std::string)> _callback_message;
typedef std::function<void(bool)> _callback_status;

using namespace arcirk;

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
    void open(const char* host, const char* port, _callback_message& msg, _callback_status& st);
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
    void set_user_uuid();
    void set_uuid();
    std::string get_client_info();

    void error(const std::string &what, const std::string &err);

    static boost::uuids::uuid string_to_uuid(const std::string& uuid){return string_to_uuid(uuid);};
    static std::string get_hash(const std::string& name,
                                const std::string& pwd){return get_sha1(name + pwd);};
    void subscribe_to_channel(const std::string &uuid_channel, const std::string &uuid_form);
    void subscribe_exit_channel(const std::string &uuid_channel, const std::string &uuid_form);
    void set_session_param(const std::string &uuid, const std::string &name, const std::string &pwd, const std::string &app_name, const std::string &user_uuid);

    void send_command(const std::string &cmd, const std::string &uuid_form, const std::string &json_param);

    //void set_status_callback(_status_changed& callback);

    bool decode_message;

    bool is_login(){return _is_login;}

private:

    boost::asio::io_context &ioc;
    bool _started;
    boost::uuids::uuid uuid_{};
    std::string name_;
    _callback_message _callback_msg;
    _callback_status status_changed;
    std::string _client_param;
    std::string _app_name;
    boost::uuids::uuid  _user_uuid{};
    bool _is_login;

    static void console_log(const std::string logText){
        std::cout << logText << std::endl;
    }

    void send_command(const std::string &cmd, const std::string &uuid_form, const std::string &param, session * sess);

    void set_param(ptree& pt);
};
