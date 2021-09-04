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

#include "./include/listener.hpp"
#include "./include/shared_state.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

#include <arc_json.h>
#include <ip.h>

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

        arc_json::ws_json json{};

        if (json.from_file(filename)){
            json.getMember("name", name);
            json.getMember("host", host);
            json.getMember("port", port);
            json.getMember("root_dir", root_dir);
            json.getMember("debug_dir", debug_dir);
            return true;
        }
        return false;
    }

    std::string to_string() const{

        arc_json::ws_json json{};
        json.set_object();
        json.addMember(arc_json::content_value("name", name));
        json.addMember(arc_json::content_value("host", host));
        json.addMember(arc_json::content_value("port", (long int)port));
        json.addMember(arc_json::content_value("root_dir", root_dir));
        json.addMember(arc_json::content_value("debug_dir", debug_dir));
        return json.to_string();
    }

    bool parse(const std::string& text){
        arc_json::ws_json json{};
        json.parse(text);
        if (json.is_parse()){
            json.getMember("name", name);
            json.getMember("host", host);
            json.getMember("port", port);
            json.getMember("root_dir", root_dir);
            json.getMember("debug_dir", debug_dir);
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
    if (!st.get_settings("../app/config/conf.json")){
        std::cerr <<
                  "Error: Ошибка чтения настроек сервера!\n";
        return EXIT_FAILURE;
    }

    std::string host = arc_json::get_default_host(st.host);

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
