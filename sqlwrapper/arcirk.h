//
// Created by admin on 22.05.2022.
//

#ifndef WS_SOLUTION_ARCIRK_H
#define WS_SOLUTION_ARCIRK_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>

namespace arcirk{

    const std::string base64_padding[] = {"", "==", "="};

    std::string get_sha1(const std::string& p_arg);
    std::string get_hash(const std::string& usr, const std::string& pwd);
    std::string base64_encode(const std::string &s);
    std::string base64_decode(const std::string &s);

}



#endif //WS_SOLUTION_ARCIRK_H
