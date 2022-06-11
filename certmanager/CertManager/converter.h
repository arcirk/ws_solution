#ifndef CONVERTER_H
#define CONVERTER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#ifdef _WINDOWS
    #pragma warning(disable:4100)
    #pragma warning(disable:4267)
    #pragma comment(lib, "advapi32")
#endif

using std::wcout;
using std::wstring;
using std::vector;

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

typedef unsigned char _BYTE;
typedef std::vector<_BYTE> ByteArray;

namespace  Base64Converter{

    static inline bool byte_is_base64(_BYTE c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    static inline std::string byte_to_base64(_BYTE const* buf, unsigned int bufLen) {
        std::string ret;
        int i = 0;
        int j = 0;
        _BYTE char_array_3[3];
        _BYTE char_array_4[4];

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


    static inline ByteArray base64_to_byte(std::string const& encoded_string) {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        _BYTE char_array_4[4], char_array_3[3];
        std::vector<_BYTE> ret;

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

    static inline void writeFile(const std::string& filename, ByteArray& file_bytes){
        std::ofstream file(filename, std::ios::out|std::ios::binary);
        std::copy(file_bytes.cbegin(), file_bytes.cend(),
            std::ostream_iterator<unsigned char>(file));
    }

    static inline void readFile(const std::string &filename, ByteArray &result)
    {

        FILE * fp = fopen(filename.c_str(), "rb");

        fseek(fp, 0, SEEK_END);
        size_t flen= ftell(fp);
        fseek(fp, 0, SEEK_SET);

        std::vector<unsigned char> v (flen);

        fread(&v[0], 1, flen, fp);

        fclose(fp);

        result = v;
    }

}


#endif // CONVERTER_H
