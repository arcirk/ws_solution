#ifndef ARCIRK_LIBRARY_H
#define ARCIRK_LIBRARY_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <boost/exception/all.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>

typedef rapidjson::GenericDocument<rapidjson::UTF8<> > _Document;
typedef rapidjson::GenericValue<rapidjson::UTF8<> > _Value;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<> > _StringBuffer;

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
using boost::property_tree::ptree;
using namespace rapidjson;

static constexpr time_t const NULL_TIME = -1;

namespace arcirk{

    typedef boost::variant<std::string, long int, bool, double, boost::uuids::uuid> _Variant;
    typedef std::string             T_str;
    typedef std::vector<T_str>      T_vec;

    bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result);
    std::string get_sha1(const std::string& p_arg);
    std::string get_hash(const std::string& usr, const std::string& pwd);
    boost::uuids::uuid string_to_uuid(const std::string& sz_uuid, bool random_uuid = false);
    std::string nil_string_uuid();
    boost::uuids::uuid nil_uuid();
    std::string uuid_to_string(const boost::uuids::uuid& uuid);
    std::string base64_encode(const std::string &s);
    std::string base64_decode(const std::string &s);
    int split_str_to_vec(const T_str s, const T_str DELIM, T_vec& v);
    T_vec split(T_str line, T_str sep);
    long int current_date_seconds();
    long int tz_offset(time_t when = NULL_TIME);
    std::string random_uuid();
    long int add_day(const long int selDate, const int dayCount);

    std::string _crypt(const std::string &source, const std::string& key);
    void* crypt(void* data, unsigned data_size, void* key, unsigned key_size);

    bool verify_conf_directory();
    std::string get_conf_directory();
    std::string get_conf_dirname();
    bool verify_directory(const std::string& dir_path);
    std::string get_home();

    template<typename... Arguments>
    std::string str_sample(const std::string& fmt, const Arguments&... args){return boost::str((boost::format(fmt) % ... % args));}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    class bVariant{
        public:
            bVariant(const std::string& val):value(std::move(val)){}
            bVariant(const char* val):value(std::string(std::move(val))){}
            bVariant(long int val):value((long int)val){}
            bVariant(double val):value((double)val){}
            bVariant(int val):value((long int)val){}
            bVariant(bool val):value((bool)val){}
            bVariant(boost::uuids::uuid val):value(val){}
            bVariant() = default;

            std::string get_string();
            long int get_int();
            double get_double();
            bool get_bool();
            bool is_bool();
            bool is_string();
            bool is_int();
            bool is_uuid();
            boost::uuids::uuid get_uuid();
            bool is_double();
            std::string to_string();

        private:
            _Variant value;

    };

    typedef struct content_value {
        std::string key;
        bVariant value;

        content_value(const std::string& key_, const bVariant& val)
                : key(std::move(key_)) { value = val; }

        content_value() = default;

    } content_value;

    class bJson : public _Document {

    public:

        bool from_file(const std::string& filename);
        bool to_file(const std::string& filename);
        //
        //        bool getMember(const std::string &member, std::string &result);
        //        bool getMember(const std::string &member, int &result);
        bool getMember(const std::string &member, bVariant &value);
        bVariant get_member(const std::string &member);
        //        std::string getStringMember(const std::string &member);
        static bool getMembers(_Document *doc, const std::string &member, std::vector<content_value> &value);
        bool getMembers(const std::string &member, std::vector<content_value> &value);
        //        bool getMembers(const std::string &member, std::vector <std::vector<content_value>> &value);
        //
        static void addMember(_Document * doc, content_value& value);
        static void addMember(_Document *doc, content_value &val, bool updateIsExists);

        void addMember(content_value value) { addMember(this, value); };
        void addMember(content_value value, bool updateIsExists) { addMember(this, value, updateIsExists); };
        void addMember(_Value* object, content_value val);
        void addMember(const std::string& key, _Value& value);
        void addMember(const std::string &member, bVariant value);
        //void addMember(const std::string &member, bVariant &value);

        //        void addObjectMember(_Value * object, content_value val);
        void addObject(_Document * doc, std::vector<content_value> &values);
        void addObject(std::vector<content_value> &values);
        //
        static void set_object(_Document *doc);
        void set_object(){set_object(this);};
        static void set_array(_Document *doc);
        void set_array(){set_array(this);};

        static std::string to_string(_Document * doc);
        std::string to_string() {return to_string(this); };
        bool parse(const std::string &jsonText);

        static bool parse_pt(const std::string& json, ptree& pt);
        static std::string get_pt_member(ptree& pt, const std::string& key);
        //
        //        _Value::ConstMemberIterator MemberBegin_(){return this->MemberBegin();};
        //        _Value::ConstMemberIterator MemberEnd_(){return this->MemberEnd();};
        //
        //        bool is_object(){return this->IsObject();};
        //        bool is_array(){return this->IsArray();};
        //
        void push_back(_Value &value);
        void push_back(_Value &arr, bVariant& val);
        void push_back(_Value &arr, _Value& val);
        void copy_from(_Value& val);
        //
        bool is_parse();

        unsigned int get_member_count();

    private:
        bool is_parse_;

    };

    class bIp{
        public:
            static boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr);
            static std::vector<boost::asio::ip::address> get_local_interfaces();
            static std::string get_default_host(const std::string& def_host);
    };

    //namespace websockets{

        typedef std::function<void(bVariant&)> setFun;

        typedef struct _ws_message{
        public:
            boost::uuids::uuid  uuid;
            std::string         name;
            boost::uuids::uuid  uuid_channel;
            std::string         message;
            boost::uuids::uuid  uuid_form;
            std::string         command;
            std::string         hash;
            std::string         app_name;
            std::string         result;
            std::string         role;
            std::string         presentation;
            boost::uuids::uuid  uuid_user;
            std::string         contentType;
            std::string         channel_name;
            std::string         object_name;
            std::string         msg_ref;

            _ws_message();

            static std::vector<std::string> get_fields();
            void set_value(const std::string& nameField, bVariant& val);

        private:
            void set_uuid(bVariant& val);
            void set_uuid_user(bVariant& val);
            void set_uuid_form(bVariant& val);
            void set_uuid_channel(bVariant& val);
            void set_name(bVariant& val);
            void set_message(bVariant& val);
            void set_command(bVariant& val);
            void set_hash(bVariant& val);
            void set_app_name(bVariant& val);
            void set_result(bVariant& val);
            void set_role(bVariant& val);
            void set_presentation(bVariant& val);
            void set_contentType(bVariant& val);
            void set_channel_name(bVariant& val);
            void set_object_name(bVariant& val);
            void set_msg_ref(bVariant& val);

            setFun get_set_function(const std::string& nameField);

        }_ws_message;

        class ws_message{
            public:
                explicit
                ws_message(_ws_message &defMessage);

                explicit
                ws_message(const std::string &jsonText);

                explicit
                ws_message();

                std::string get_json(bool to_base64_);

                bool is_parse();

                std::string to_string();

                _ws_message& message();

            private:
                _ws_message m_message;
                bJson m_doc{};
        };

    enum bConfFields{
        ServerHost = 0,
        ServerPort,
        User,
        Hash,
        ServerName,
        AppName,
        AutoConnect,
        SaveHash,
        ServerWorkingDirectory,
        ClientWorkingDirectory,
        isLocalInstallation,
        RunAsService,
        UseLocalWebDavDirectory,
        LocalWebDavDirectory,
        WebDavHost,
        WebDavUser,
        WebDavPwd,
        WebDavSSL
    };

    class bConf{
    public:
        explicit
        bConf(const std::string& file_name = "", bool public_settings = false, bool no_save_def = false);

        bVariant & operator[] (bConfFields index);
        bVariant const& operator[] (bConfFields index) const;

        void save();
        bool parse();
        bVariant get(bConfFields index) const;
        void set(bConfFields index, const bVariant& value);

        static std::string get_field_alias(bConfFields val);
#ifdef _WINDOWS
        static LPTSTR ExtractFilePath(LPCTSTR FileName, LPTSTR buf);
#endif
        std::string parent_path() const;

    private:
        std::string output_directory;
        std::string output_filename;
        void init(bool server);

        std::vector<bVariant> m_vec;
    };


   // } //websockets
}





#endif //ARCIRK_LIBRARY_H
