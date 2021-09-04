#ifndef ARC_JSON_ARC_JSON_H
#define ARC_JSON_ARC_JSON_H

#include "global.h"
#include <unordered_set>
#include <mutex>
#include <utility>

namespace arc_json{

    class ws_json : _Document {

    public:

        bool from_file(const std::string& filename);

        bool getMember(const std::string &member, std::string &result);
        bool getMember(const std::string &member, int &result);
        bool getMember(const std::string &member, boost::variant<int, bool, std::string> &value);
        std::string getStringMember(const std::string &member);
        static bool getMembers(_Document *doc, const std::string &member, std::vector<content_value> &value);
        bool getMembers(const std::string &member, std::vector<content_value> &value);
        bool getMembers(const std::string &member, std::vector <std::vector<content_value>> &value);

        static void addMember(_Document * doc, content_value& value);
        void addMember(content_value value) { addMember(this, value); };
        void addObjectMember(_Value * object, content_value val);

        static void set_object(_Document *doc);
        void set_object(){set_object(this);};
        static void set_array(_Document *doc);
        void set_array(){set_array(this);};
        void push_back(_Value &value);

        bool is_parse() const { return is_parse_; };
        static std::string to_string(_Document * doc);
        std::string to_string() {return to_string(this); };
        bool parse(const std::string &jsonText);

        _Value::ConstMemberIterator MemberBegin_(){return this->MemberBegin();};
        _Value::ConstMemberIterator MemberEnd_(){return this->MemberEnd();};

        bool is_object(){return this->IsObject();};
        bool is_array(){return this->IsArray();};

        unsigned int get_member_count();

    private:
        bool is_parse_;
    };

    class ws_message{

        boost::uuids::uuid  uuid;
        std::string         name;
        boost::uuids::uuid  uuid_channel;
        std::string         message;
        std::string         uuid_form;
        std::string         command;
        std::string         hash;
        std::string         app_name;
        std::string         result_;
        std::string         role_;
        std::string         pref_;
        boost::uuids::uuid  user_uuid_;

    public:
        bool is_parse;

        explicit
        ws_message(
                const boost::uuids::uuid& uuid_,
                std::string  name_,
                std::string  message_,
                std::string command_,
                std::string app_name_,
                const boost::uuids::uuid& uuid_channel_ = boost::uuids::nil_uuid(),
                std::string  _result = "ok",
                std::string  _role = "user",
                const boost::uuids::uuid& user_uuid = boost::uuids::nil_uuid(),
                std::string  _uuid_form = "00000000-0000-0000-0000-000000000000",
                std::string  pref = "")
        : uuid(uuid_)
        , name(std::move(name_))
        , uuid_channel(uuid_channel_)
        , message(std::move(message_))
        , uuid_form(std::move(_uuid_form))
        , command(std::move(command_))
        , hash()
        , app_name(std::move(app_name_))
        , result_(std::move(_result))
        , role_(std::move(_role))
        , pref_(std::move(pref))
        , user_uuid_(user_uuid)
        {
            is_parse = false;
//            if (user_uuid.empty() || user_uuid == "00000000-0000-0000-0000-000000000000"){
//                user_uuid_ = boost::uuids::random_generator()();
//            } else{
//                if (!arc_json::is_valid_uuid(user_uuid, user_uuid_)){
//                    user_uuid_ = boost::uuids::random_generator()();
//                }
//            }
        }

        explicit
        ws_message(const std::string&  message_)
                : uuid()
                , name("")
                , uuid_channel()
                , message("")
                , uuid_form("00000000-0000-0000-0000-000000000000")
                , command("message")
                , hash()
                , app_name()
                , result_("ok")
                , role_("user")
                , pref_("")
                , user_uuid_(boost::uuids::random_generator()())
        {
            ws_json json{};
            json.parse(message_);

            if (json.is_parse()){
                boost::variant<int, bool, std::string> uuid_s;
                if (json.getMember("uuid", uuid_s)) {
                    std::string sz_uuid = boost::get<std::string>(uuid_s);
                    if (is_valid_uuid(sz_uuid, uuid)){
                        //success
                    }
                    //uuid = gen(boost::get<std::string>(sz_uuid));
                }
                boost::variant<int, bool, std::string> name_s;
                if (json.getMember("name", name_s)) {
                    name = boost::get<std::string>(name_s);
                }
                boost::variant<int, bool, std::string> message_s;
                if (json.getMember("message", message_s)) {
                    message = boost::get<std::string>(message_s);
                }
                boost::variant<int, bool, std::string> uuid_channel_s;
                if (json.getMember("uuid_channel", uuid_channel_s)) {
                    std::string uuid_channel_ = boost::get<std::string>(uuid_channel_s);
                    if (is_valid_uuid(uuid_channel_, uuid_channel)){
                        //success
                    }
                }
                boost::variant<int, bool, std::string> uuid_form_s;
                if (json.getMember("uuid_form", uuid_form_s)) {
                    uuid_form = boost::get<std::string>(uuid_form_s);
                }
                boost::variant<int, bool, std::string> hash_s;
                if (json.getMember("hash", hash_s)) {
                    hash = boost::get<std::string>(hash_s);
                }
                boost::variant<int, bool, std::string> app_name_s;
                if (json.getMember("app_name", app_name_s)) {
                    app_name = boost::get<std::string>(app_name_s);
                }
                boost::variant<int, bool, std::string> result_s;
                if (json.getMember("result", result_s)) {
                    result_ = boost::get<std::string>(result_s);
                }
                boost::variant<int, bool, std::string> role_s;
                if (json.getMember("role", role_s)) {
                    role_ = boost::get<std::string>(role_s);
                }
                boost::variant<int, bool, std::string> pref_s;
                if (json.getMember("pref", pref_s)) {
                    pref_ = boost::get<std::string>(pref_s);
                }
                boost::variant<int, bool, std::string> uuid_usr_s;
                if (json.getMember("user_uuid", uuid_usr_s)) {
                    std::string uuid_usr_ss = boost::get<std::string>(uuid_usr_s);
                    if (!arc_json::is_valid_uuid(uuid_usr_ss, user_uuid_)){
                            user_uuid_ = boost::uuids::random_generator()();
                    }
                }
                boost::variant<int, bool, std::string> command_s;
                if (json.getMember("command", command_s)) {
                    command = boost::get<std::string>(command_s);
                }
                is_parse = true;
            }
        }

        std::string get_json(bool to_base64_){

            ws_json json{};
            json.set_object();
            json.addMember(content_value("uuid", boost::lexical_cast<std::string>((uuid))));
            json.addMember(content_value("name", name));
            json.addMember(content_value("uuid_channel", boost::lexical_cast<std::string>((uuid_channel))));
            json.addMember(content_value("message", message));
            json.addMember(content_value("uuid_form", uuid_form));
            json.addMember(content_value("command", command));
            json.addMember(content_value("result", result_));
            json.addMember(content_value("app_name", app_name));
            json.addMember(content_value("role", role_));
            json.addMember(content_value("user_uuid", boost::lexical_cast<std::string>(user_uuid_)));

            std::string result = json.to_string();

            if (!to_base64_)
                return result;
            else
                return base64_encode(result);
        };

        std::string& get_message(){
            return message;
        }

        boost::uuids::uuid& get_uuid(){
            return uuid;
        }

        std::string& get_name(){
            return name;
        }

        std::string& get_hash(){
            return hash;
        }

        std::string get_role(){
            return role_;
        }
        std::string get_pref(){
            return pref_;
        }
        boost::uuids::uuid& get_user_uuid(){
            return user_uuid_;
        }
        boost::uuids::uuid& get_uuid_channel(){
            return uuid_channel;
        }
        std::string get_uuid_form(){
            return uuid_form;
        }

        const std::string& get_app_name(){
            return app_name;
        }

        const std::string& get_command(){
            return command;
        }

    };


}



#endif //ARC_JSON_ARC_JSON_H
