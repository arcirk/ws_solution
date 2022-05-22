//
// Created by admin on 22.05.2022.
//
#include "arcirk.h"

namespace arcirk{

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

        }catch (std::exception& e){
            std::cerr << "error: " << e.what() << std::endl;
            return "";
        }
    }
}