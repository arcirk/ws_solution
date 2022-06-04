//
// Created by arcady on 05.11.2021.
//

#include "../include/arcirk.h"
#include <boost/format.hpp>
#ifdef _WINDOWS
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
const std::string base64_padding[] = {"", "==", "="};
typedef boost::variant<std::string, long int, bool, double, boost::uuids::uuid> _Variant;
static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


#ifdef _WINDOWS
using namespace std::filesystem;
#else
using namespace boost::filesystem;
#endif

namespace arcirk{

    bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result) {
        using namespace boost::uuids;

        try {
            result = string_generator()(maybe_uuid);
            return result.version() != uuid::version_unknown;
        } catch(...) {
            return false;
        }
    }

    std::string get_sha1(const std::string& p_arg)
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

    std::string get_hash(const std::string& usr, const std::string& pwd){
        std::string _usr(usr);
        const std::string& _pwd(pwd);

        boost::trim(_usr);
        boost::to_upper(_usr);

        return get_sha1(_usr + _pwd);
    }

    std::string nil_string_uuid(){
        return "00000000-0000-0000-0000-000000000000";
    }

    boost::uuids::uuid string_to_uuid(const std::string& sz_uuid, bool random_uuid) {

        if (sz_uuid == nil_string_uuid() && random_uuid)
            return boost::uuids::random_generator()();

        boost::uuids::uuid uuid{};

        if (is_valid_uuid(sz_uuid, uuid)){
            return uuid;
        }

        if (!random_uuid)
            return boost::uuids::nil_uuid();
        else
            return boost::uuids::random_generator()();
    }

    boost::uuids::uuid nil_uuid(){
        return boost::uuids::nil_uuid();
    }

    std::string uuid_to_string(const boost::uuids::uuid& uuid){
        try {
            return boost::to_string(uuid);
        } catch(...) {
            return nil_string_uuid();
        }
        //return boost::lexical_cast<std::string>(uuid);
    }

    std::string base64_encode(const std::string &s) {
        namespace bai = boost::archive::iterators;

        try {
            std::stringstream os;

            // convert binary values to base64 characters
            typedef bai::base64_from_binary
            // retrieve 6 bit integers from a sequence of 8 bit bytes
                    <bai::transform_width<const char *, 6, 8>> base64_enc; // compose all the above operations in to a new iterator

            std::copy(base64_enc(s.c_str()), base64_enc(s.c_str() + s.size()),
                      std::ostream_iterator<char>(os));

            os << base64_padding[s.size() % 3];
            return os.str();
        }catch (std::exception& e){
            std::cerr << "error: " << e.what() << std::endl;
            return "";
        }

    }

    std::string base64_decode(const std::string &s) {
        namespace bai = boost::archive::iterators;

        try {
            std::stringstream os;

            typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;

            unsigned int size = (unsigned int)s.size();

            // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
            if (size && s[size - 1] == '=') {
                --size;
                if (size && s[size - 1] == '=') --size;
            }
            if (size == 0) return std::string();

            std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
                      std::ostream_iterator<char>(os));

            return os.str();

        }catch (std::exception& e){
            std::cerr << "error: " << e.what() << std::endl;
            return "";
        }
    }

    int split_str_to_vec(const T_str s, const T_str DELIM, T_vec& v)
    {
        size_t l, r;

        for (l = s.find_first_not_of(DELIM), r = s.find_first_of(DELIM, l);
             l != std::string::npos; l = s.find_first_not_of(DELIM, r), r = s.find_first_of(DELIM, l))
            v.push_back(s.substr(l, r - l));
        return (int)v.size();
    }

    T_vec split(T_str line, T_str sep)
    {
        T_vec  v;

        split_str_to_vec(line, sep, v);

        //v.begin();

        return v;
    }

    long int current_date_seconds() {

        tm current{};
        time_t t = time(nullptr);

#ifdef _WIN32
        localtime_s(&current, &t);
#else
        localtime_r(&t, &current);
#endif

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&current));

        return
                (long int)std::chrono::duration_cast<std::chrono::seconds>(
                        tp.time_since_epoch()).count();

    }

    long int tz_offset(time_t when)
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

    std::string random_uuid(){
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        return uuid_to_string(uuid);
    }

    long int add_day(const long int selDate, const int dayCount){
        return selDate + (dayCount * (60*60*24));
    }

    std::string get_home(){
#ifdef _WINDOWS
        return getenv("APPDATA");
#else
        return getenv("HOME");
#endif
    }

    bool verify_directory(const std::string& dir_path) {

        path p(dir_path);

        if (!exists(p)) {
#ifdef _WINDOWS
            return std::filesystem::create_directories(p);
#else
            return boost::filesystem::create_directories(p);
#endif
        }

        return true;
    }

    std::string get_conf_dirname(){
#ifdef _WINDOWS
        return "arcirk";
#else
        return ".arcirk";
#endif
    }

    std::string get_conf_directory(){
#ifdef _WINDOWS
        return get_home() + '\\' + get_conf_dirname();
#else
        return get_home() + path::separator + get_conf_dirname();
#endif
    }

    bool verify_conf_directory(){

        bool result = verify_directory(get_conf_directory());
        return result;
    }

    void* crypt(void* data, unsigned data_size, void* key, unsigned key_size)
    {
        assert(data && data_size);
        if (!key || !key_size) return data;

        auto* kptr = (uint8_t*)key; // начало ключа
        uint8_t* eptr = kptr + key_size; // конец ключа

        for (auto* dptr = (uint8_t*)data; data_size--; dptr++)
        {
            *dptr ^= *kptr++;
            if (kptr == eptr) kptr = (uint8_t*)key; // переход на начало ключа
        }
        return data;
    }

    std::string _crypt(const std::string &source, const std::string& key) {

        void * text = (void *) source.c_str();
        void * pass = (void *) key.c_str();
        crypt(text, ARR_SIZE(source.c_str()), pass, ARR_SIZE(key.c_str()));

        std::string result((char*)text);


        return result;
    }

    std::string byte_to_base64(BYTE const* buf, unsigned int bufLen) {
        std::string ret;
        int i = 0;
        int j = 0;
        BYTE char_array_3[3];
        BYTE char_array_4[4];

        while (bufLen--) {
            char_array_3[i++] = *(buf++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = 0;

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    std::vector<BYTE> base64_to_byte(std::string const& encoded_string) {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        BYTE char_array_4[4], char_array_3[3];
        std::vector<BYTE> ret;

        while (in_len-- && ( encoded_string[in_] != '=') && byte_is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i ==4) {
                for (i = 0; i <4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j <4; j++)
                char_array_4[j] = 0;

            for (j = 0; j <4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
        }

        return ret;
    }
}
