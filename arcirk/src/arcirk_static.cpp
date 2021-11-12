//
// Created by arcady on 05.11.2021.
//

#include "../include/arcirk.h"

#include <boost/format.hpp>

const std::string base64_padding[] = {"", "==", "="};
typedef boost::variant<std::string, long int, bool, double, boost::uuids::uuid> _Variant;

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

        if (sz_uuid == nil_string_uuid()&& random_uuid)
            return boost::uuids::random_generator()();

        boost::uuids::uuid uuid{};

        if (is_valid_uuid(sz_uuid, uuid)){
            return uuid;
        }

        if (!random_uuid)
            return boost::uuids::uuid();
        else
            return boost::uuids::random_generator()();
    }



    boost::uuids::uuid nil_uuid(){
        return boost::uuids::nil_uuid();
    }

    std::string uuid_to_string(const boost::uuids::uuid& uuid){
        return boost::lexical_cast<std::string>(uuid);
    }

    std::string base64_encode(const std::string &s) {
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

    std::string base64_decode(const std::string &s) {
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

        v.begin();

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
                std::chrono::duration_cast<std::chrono::seconds>(
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

    template<typename... Arguments>
    std::string str_sample(const std::string& fmt, const Arguments&... args)
    {
//        result:
//        std::cout << FormatArgs("no args\n"); // "no args"
//        std::cout << FormatArgs("%s; %s; %s;\n", 123, 4.3, "foo"); // 123; 4.3; foo;
//        std::cout << FormatArgs("%2% %1% %2%\n", 1, 12); // 12 1 12

        boost::format f(fmt);
        std::initializer_list<char> {(static_cast<void>(
                f % args
        ), char{}) ...};

        return boost::str(f);
    }

}