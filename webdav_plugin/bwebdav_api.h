//
// Created by admin on 23.01.2022.
//

#ifndef BWEBDAV_TEST_BWEBDAV_API_H
#define BWEBDAV_TEST_BWEBDAV_API_H

#include <boost/config.hpp>
#include <string>

class BOOST_SYMBOL_VISIBLE bwebdav_api{
public:
    virtual bool check_connection() = 0;

    virtual ~bwebdav_api()= default;;
};

#endif //BWEBDAV_TEST_BWEBDAV_API_H
