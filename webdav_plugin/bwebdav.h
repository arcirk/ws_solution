#ifndef WEBDAV_BWEBDAV_H
#define WEBDAV_BWEBDAV_H

#include <boost/config.hpp>
#include "bwebdav_api.h"

namespace arcirk{

    class bWebDav : bwebdav_api{

    public:
        bWebDav();

        bool  check_connection() override;

    private:
//        std::unique_ptr<WebDAV::Client> client;
//        std::map<std::string, std::string> options =
//                {
//                        {"webdav_hostname", "https://webdav.yandex.ru"},
//                        {"webdav_username", "webdav_username"},
//                        {"webdav_password", "webdav_password"}
//                };
    };

    extern "C" BOOST_SYMBOL_EXPORT bWebDav plugin;
    bWebDav plugin;
}



#endif //WEBDAV_BWEBDAV_H
