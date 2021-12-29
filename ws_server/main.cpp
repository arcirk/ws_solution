//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

//------------------------------------------------------------------------------
/*
    WebSocket chat server, multi-threaded

    This implements a multi-user chat room using WebSocket. The
    `io_context` runs on any number of threads, specified at
    the command line.

*/
//------------------------------------------------------------------------------

#ifdef _WINDOWS
#include <SDKDDKVer.h>
#endif

#include "./include/listener.hpp"
#include "./include/shared_state.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

#include <arcirk.h>

//#include "./include/base.h"
#include <string>
//#include <sqlext.h>
//#include <sqltypes.h>
//#include <sql.h>
//#include <stdio.h>

typedef struct settings{

    std::string name;
    std::string host;
    int port;
    std::string root_dir;
    std::string debug_dir;

    bool get_settings(const std::string& filename){

        arcirk::bJson json{};

        if (json.from_file(filename)){
            name = json.get_member("name").get_string();
            host = json.get_member("host").get_string();
            port = json.get_member("port").get_int();
            root_dir = json.get_member("root_dir").get_string();
            debug_dir = json.get_member("debug_dir").get_string();
            return true;
        }
        return false;
    }

    std::string to_string() const{

        arcirk::bJson json{};
        json.set_object();
        json.addMember(arcirk::content_value("name", name));
        json.addMember(arcirk::content_value("host", host));
        json.addMember(arcirk::content_value("port", (long int)port));
        json.addMember(arcirk::content_value("root_dir", root_dir));
        json.addMember(arcirk::content_value("debug_dir", debug_dir));
        return json.to_string();
    }

    bool parse(const std::string& text){
        arcirk::bJson json{};
        json.parse(text);
        if (json.is_parse()){
            name = json.get_member("name").get_string();
            host = json.get_member("host").get_string();
            port = json.get_member("port").get_int();
            root_dir = json.get_member("root_dir").get_string();
            debug_dir = json.get_member("debug_dir").get_string();
            return true;
        }
        return false;
    }

}settings;

int
main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    settings st;
    if (!st.get_settings("config/conf.json")){
        std::cerr <<
                  "Error: Ошибка чтения настроек сервера!\n";
        return EXIT_FAILURE;
    }

    std::string host = arcirk::bIp::get_default_host(st.host);

    std::cout << "Start server " << host << ":" << st.port << std::endl;

    auto const threads = 4;
    auto address = net::ip::make_address(host);
    auto port = static_cast<unsigned short>(st.port);
    auto doc_root = st.root_dir;

    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    boost::make_shared<listener>(
            ioc,
            tcp::endpoint{address, port},
            boost::make_shared<shared_state>(doc_root + "html/"))->run();//, boost::make_shared<channel>())->run();

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
            [&ioc](boost::system::error_code const&, int)
            {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                ioc.stop();
            });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
                [&ioc]
                {
                    ioc.run();
                });
    ioc.run();

    // (If we get here, it means we got a SIGINT or SIGTERM)

    // Block until all the threads exit
    for(auto& t : v)
        t.join();

    return EXIT_SUCCESS;
}
