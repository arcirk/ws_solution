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

#include <string>

int
main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    arcirk::bConf st = arcirk::bConf("conf.json", true);

    std::string host = arcirk::bIp::get_default_host(st[arcirk::bConfFields::ServerHost].get_string());

    if (host != st[arcirk::bConfFields::ServerHost].get_string()){
        st[arcirk::bConfFields::ServerHost] = host;
        st.save();
    }

    std::cout << "Start server " << host << ":" << st[arcirk::bConfFields::ServerPort].to_string() << std::endl;

    auto const threads = 4;
    auto address = net::ip::make_address(host);
    auto port = static_cast<unsigned short>(st[arcirk::bConfFields::ServerPort].get_int());
    auto rootDir = st[arcirk::bConfFields::ServerWorkingDirectory].to_string();

    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    boost::make_shared<listener>(
            ioc,
            tcp::endpoint{address, port},
            boost::make_shared<shared_state>(rootDir + "/html/"))->run();//, boost::make_shared<channel>())->run();

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
