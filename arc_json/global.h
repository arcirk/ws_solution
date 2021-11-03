//
// Created by arcady on 24.07.2021.
//

#ifndef ARC_JSON_SOLUTION_GLOBAL_H
#define ARC_JSON_SOLUTION_GLOBAL_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
#include <fstream>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

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

typedef boost::variant<std::string, long int, bool, double> bVariant;

using namespace rapidjson;

namespace arc_json{

    typedef struct content_value {
        std::string key;
        boost::variant<std::string, long int, bool, double, std::vector<content_value*>, content_value*> value;

        content_value(std::string key_, long int val)
                : key(std::move(key_)) { value = val; }

        content_value(std::string key_, bool val)
                : key(std::move(key_)) { value = val; }

        content_value(std::string key_, std::string val)
                : key(std::move(key_)),
                  value(val)
        {
        }

        content_value() = default;

    } content_value;
    static constexpr time_t const NULL_TIME = -1;

    const std::string base64_padding[] = {"", "==", "="};
    typedef std::string             T_str;
    typedef std::vector<T_str>      T_vec;
    typedef std::function<void(std::string)> _command;

    static std::string remove_text_enclosed_in_quotation_marks(const  std::string& text){
        boost::regex reg("(\").*?\\1");
        return boost::regex_replace(text, reg, "");
    }

    static int in_quotation_marks_to_vec(const T_str& s, T_vec& v){
        boost::regex re("\"([^\"]+)\"");
        boost::sregex_iterator m1(s.begin(), s.end(), re);
        boost::sregex_iterator m2;
        for (; m1 != m2; ++m1) {
            v.push_back((*m1)[1].str());
        }
        return (int)v.size();
    }

    static T_vec get_text_in_quotation_marks(const T_str& line){
        T_vec  v;

        int size = in_quotation_marks_to_vec(line, v);

        auto i = v.begin();

        return v;
    }

    static bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result) {
        using namespace boost::uuids;

        try {
            result = string_generator()(maybe_uuid);
            return result.version() != uuid::version_unknown;
        } catch(...) {
            return false;
        }
    }

    static std::string base64_encode(const std::string &s) {
        namespace bai = boost::archive::iterators;

        std::stringstream os;

        // convert binary values to base64 characters
        typedef bai::base64_from_binary
        // retrieve 6 bit integers from a sequence of 8 bit bytes
                <bai::transform_width<const char *, 6, 8>> base64_enc; // compose all the above operations in to a new iterator

        std::copy(base64_enc(s.c_str()), base64_enc(s.c_str() + s.size()),
                  std::ostream_iterator<char>(os));

        os << base64_padding[s.size() % 3];
        return os.str();
    }

    static std::string base64_decode(const std::string &s) {
        namespace bai = boost::archive::iterators;

        std::stringstream os;

        typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;

        unsigned int size = s.size();

        // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
        if (size && s[size - 1] == '=') {
            --size;
            if (size && s[size - 1] == '=') --size;
        }
        if (size == 0) return std::string();

        std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
                  std::ostream_iterator<char>(os));

        return os.str();
    }

    static int split_str_to_vec(const T_str s, const T_str DELIM, T_vec& v)
    {
        size_t l, r;

        for (l = s.find_first_not_of(DELIM), r = s.find_first_of(DELIM, l);
             l != std::string::npos; l = s.find_first_not_of(DELIM, r), r = s.find_first_of(DELIM, l))
            v.push_back(s.substr(l, r - l));
        return (int)v.size();
    }

    static T_vec split(T_str line, T_str sep)
    {
        T_vec  v;

        int size = split_str_to_vec(line, sep, v);

        auto i = v.begin();

        return v;
    }

    static boost::uuids::uuid string_to_uuid(const std::string& sz_uuid, bool random_uuid = false) {

        if (sz_uuid == "00000000-0000-0000-0000-000000000000" && random_uuid)
            return boost::uuids::random_generator()();

        boost::uuids::uuid uuid{};

        if (arc_json::is_valid_uuid(sz_uuid, uuid)){
            return uuid;
        }

        if (!random_uuid)
            return boost::uuids::uuid();
        else
            return boost::uuids::random_generator()();
    }

    static std::string uuid_to_string(boost::uuids::uuid& uuid){
        return boost::lexical_cast<std::string>(uuid);
    }

    static std::string get_sha1(const std::string& p_arg)
    {
        boost::uuids::detail::sha1 sha1;
        sha1.process_bytes(p_arg.data(), p_arg.size());
        unsigned hash[5] = {0};
        sha1.get_digest(hash);

        // Back to string
        char buf[41] = {0};

        for (int i = 0; i < 5; i++)
        {
            std::sprintf(buf + (i << 3), "%08x", hash[i]);
        }

        return std::string(buf);
    }

    static std::string get_hash(const std::string& usr, const std::string& pwd){
        std::string _usr(usr);
        std::string _pwd(pwd);

        boost::trim(_usr);
        boost::to_upper(_usr);

        return get_sha1(_usr + _pwd);
    }

    static long int getSecondsSince1970Until(std::string dateAndHour) {

        tm tm = {};
        std::stringstream ss(dateAndHour);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&tm));


        return
                std::chrono::duration_cast<std::chrono::seconds>(
                        tp.time_since_epoch()).count();

    }

    static long int tz_offset(time_t when = NULL_TIME)
    {
        if (when == NULL_TIME)
            when = std::time(nullptr);
        auto const tm = *std::localtime(&when);
        std::ostringstream os;
        os << std::put_time(&tm, "%z");
        std::string s = os.str();
        // s is in ISO 8601 format: "±HHMM"
        int h = std::stoi(s.substr(0,3), nullptr, 10);
        int m = std::stoi(s[0]+s.substr(3), nullptr, 10);

        return (h-1) * 3600 + m * 60;
    }

    static long int current_date_seconds() {

        tm current{};
        time_t t = time(nullptr);

#ifdef _WIN32
        localtime_s(&current, &t);
#else
        localtime_r(&t, &current);
#endif

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&current));

        return
                std::chrono::duration_cast<std::chrono::seconds>(
                        tp.time_since_epoch()).count();

    }

    static std::string nil_uuid(){
        return "00000000-0000-0000-0000-000000000000";
    }

}

#endif //ARC_JSON_SOLUTION_GLOBAL_H
