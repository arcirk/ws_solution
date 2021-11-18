
//#include <format>
#include <iostream>
#include "../include/iws_client.h"
#include <string>

#include <boost/format.hpp>

#ifdef _WINDOWS
#pragma warning (disable : 4505)
#pragma warning (disable : 4061)
#endif

IClient * client;

void status_changed(bool connected){
    std::cout << "connected: " << connected << std::endl;
}

void ext_message(const std::string& msg) {

    try {
            std::cout << base64_decode(msg) << std::endl;

    }catch (std::exception &e){
        std::cout << msg << std::endl;
    }


    // auto success = this->ExternalEvent("WebSocketAddIn", "message", msg);
}
//------------------------------------------------------------------------------
void start(){

    if (client->started()) {
        return;
    }


    client->admin_name = "Борисоглебский Аркадий";
    client->hash = IClient::get_hash("Борисоглебский Аркадий", "123");
    client->host = "192.168.43.4";
    client->port = 8080;
    client->app_name = "console";

    client->open();

    std::cout << "exit thread" << std::endl;

}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Russian");

    _callback_message callback = [](auto && PH1) { return ext_message(std::forward<decltype(PH1)>(PH1)); };
    _callback_status callback_status = [](auto && PH1) { return status_changed(std::forward<decltype(PH1)>(PH1)); };

    client = new IClient("192.168.43.4", 8080, callback, callback_status);
    //client->set_callback_status_changed(_callback_status_changed);

    std::string line;

    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        // Reconnect the client
        if (line == "!")
        {
            continue;
        }
        else if (line == "start")
        {
            start();

            continue;
        }
        else if (line == "stop")
        {
            client->close();
            continue;
        }
        else if (line == "exit")
        {
            if (client->started())
                client->close();
            break;
        }
        else if (line == "started")
        {
            std::cout << "started: " << client->started() << std::endl;
            continue;
        }
        else if (line == "get_catalog")
        {
            if (client->started())
                client->get_users_catalog("0");
            continue;
        }else if(line == "get_user_info"){
            if (client->started())
                client->get_user_info(client->get_user_uuid(), "");
            continue;
        }else if(line == "get_user_data"){
            if (client->started())
                client->get_user_data("d7cca261-aecc-4708-872c-6cb6a664a6d7", "", "{\"draft\":true, \"unreadMessages\":true, \"status\":true}");
            continue;
        }

    }




    //    boost::asio::thread_pool pool(4);
    //    boost::asio::post(pool, start);
    //    boost::asio::post(pool,
    //                      []()
    //                      {
    //        //
    //                      });

    // Wait for all tasks in the pool to complete.
    //   pool.join();

    delete client;

    return EXIT_SUCCESS;
}


