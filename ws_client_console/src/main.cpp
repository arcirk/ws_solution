
//#include <format>
#include <iostream>
#include <iws_client.h>
#include <server_response.h>
#include <string>

#include <boost/format.hpp>

#ifdef _WINDOWS
#pragma warning (disable : 4505)
#pragma warning (disable : 4061)
#endif

IClient * client;

void displayError(const std::string &wh, const std::string &err){
    std::cerr << wh << ":" << err << std::endl;
}

void connectionSuccess(){
    std::cout << "connectionSuccess" << std::endl;
}

void processServeResponse(const std::string &jsonResp)
{
    auto resp = new ServeResponse(jsonResp);

    if(!resp->isParse){
        delete resp;
        return;
    }
    if(resp->result == "error"){

        displayError(resp->command, resp->message);

        if(resp->command == "set_client_param")
            client->close();

    }else
    {
        if(resp->command == "set_client_param"){
            connectionSuccess();
            client->send_command("set_content_type", "", "{\"content_type\":\"HTML\"}");
        }else if (resp->command == "set_content_type"){
            client->send_command("set_message_struct_type", "", "{\"struct_type\":\"DB\"}");
        }else if (resp->command == "set_message_struct_type"){
            client->send_command("get_webdav_settings", "", "");
        }else if (resp->command == "get_users_catalog"){
            //
        }else if (resp->command == "get_user_cache"){
            //
        }else if(resp->command == "get_active_users"){
            //getActiveUsers(resp->message);
        }else if (resp->command == "get_messages"){
            //setMessages(resp->message);
        }else if (resp->command == "close_connections"){
            //closeConnection();
        }else if (resp->command == "message"){
            //messageReceived(resp->message, resp->uuid, resp->recipient, resp->recipientName);
        }else if (resp->command == "set_user_cache"){
            //
        }else if (resp->command == "get_user_info"){
            //userInfo(resp->message);
        }else if (resp->command == "client_join"){
            //clientJoin(resp->message);
        }else if(resp->command == "client_leave"){
            //clientLeave(resp->message);
        }else if(resp->command == "get_user_status"){
            //requestUserStatus(resp->message);
        }else if(resp->command == "reset_unread_messages"){
            //
        }else if(resp->command == "get_unread_messages"){
            //unreadMessages(resp->message);
        }else if(resp->command == "command_to_qt_client"){
            //
        }else if(resp->command == "get_group_list"){
            //getGroupList(resp->message);
        }else if(resp->command == "get_users"){
            //getListUsers(resp->message);
        }else if(resp->command == "add_group"){
            //addGroupUsers(resp->message);
        }else if(resp->command == "edit_group"){
            //editGroupUsers(resp->message);
        }else if(resp->command == "remove_group"){
            //removeGroupUsers(resp->message);
        }else if (resp->command == "add_user"){
            //addUser(resp->message);
        }else if (resp->command == "remove_user"){
            //deleteUser(resp->message);
        }else if (resp->command == "update_user"){
            //updateUser(resp->message);
        }else if (resp->command == "set_parent"){
            //setUserParent(resp->message);
        }else if(resp->command == "get_channel_token"){
            //
        }else if (resp->command == "get_webdav_settings"){
            //
        }else if (resp->command == "set_webdav_settings"){
            //обновились настройки webdav на сервере
        }else if (resp->command == "command_to_qt_agent"){
            //
        }
        else
            std::cout << "Не известная команда: " << resp->command << std::endl;
    }

    delete resp;
}

void status_changed(bool connected){
    std::cout << "connected: " << connected << std::endl;
}

void ext_message(const std::string& msg) {

    std::string resp = ServeResponse::base64_decode(msg); arcirk::base64_decode(msg);

    if(!resp.empty()){
        processServeResponse(resp);
    }

//    try {
//            std::cout << IClient::base64_decode(msg) << std::endl;
//
//    }catch (std::exception &e){
//        std::cout << msg << std::endl;
//    }


    // auto success = this->ExternalEvent("WebSocketAddIn", "message", msg);
}
//------------------------------------------------------------------------------
void start(){

    if (client->started()) {
        return;
    }


    client->admin_name = "Борисоглебский Аркадий";
    client->hash = IClient::get_hash("Борисоглебский Аркадий", "123");
    client->host = "192.168.43.18";
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

    client = new IClient("192.168.43.18", 8080, callback, callback_status);
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


