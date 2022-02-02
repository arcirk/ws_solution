#include "bwebdav.h"
#include "../ws_webdav/lib/include/bwebdav.h"


#include <webdav/client.hpp>
#include <map>
#include <memory>

using namespace arcirk;

//bWebDav::bWebDav()
//: client(new WebDAV::Client{ options } )
//{
//
//}

bWebDav::bWebDav()
{

}

bool bWebDav::check_connection() {
//    bool _check_connection = client->check();
////    std::cout << "test connection with WebDAV drive is "
////              << (_check_connection ? "" : "not ")
////              << "successful"<< std::endl;
////    return _check_connection;

    std::map<std::string, std::string> options =
            {
                    {"webdav_hostname", "https://arcirk.ru"},
                    {"webdav_username", "webDavUser"},
                    {"webdav_password", "@LbyFvj1"},
                    {"webdav_root", "remote.php/dav/files/webDavUser"}
            };

    std::unique_ptr<WebDAV::Client> client{ new WebDAV::Client{ options } };
    bool _check_connection = client->check();
    std::cout << "test connection with WebDAV drive is "
              << (_check_connection ? "" : "not ")
              << "successful"<< std::endl;

    return _check_connection;
}
