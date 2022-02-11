
//#include <format>
#include <iostream>
#include <iws_client.h>
#include <server_response.h>
#include <string>

#include <boost/format.hpp>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#ifdef _WINDOWS
#pragma warning (disable : 4505)
#pragma warning (disable : 4061)
#pragma warning (disable : 4100)
#endif

IClient * client;

boost::mutex mtx;
boost::condition_variable cv;

void displayError(const std::string &wh, const std::string &err){
    std::cerr << wh << ":" << err << std::endl;
}

void connectionSuccess(){
    boost::unique_lock <boost::mutex> lck(mtx);
    cv.notify_one();
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
            std::cout << resp->command << ": " << resp->message << std::endl;
            connectionSuccess();
            client->send_command("set_content_type", "", "{\"content_type\":\"HTML\"}");
        }else if (resp->command == "set_content_type"){
            std::cout << resp->command << ": " << resp->message << std::endl;
            client->send_command("set_message_struct_type", "", "{\"struct_type\":\"DB\"}");
        }else if (resp->command == "set_message_struct_type"){
            std::cout << resp->command << ": " << resp->message << std::endl;
            client->send_command("get_webdav_settings", "", "");
        }else if (resp->command == "get_users_catalog"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
        }else if (resp->command == "get_user_cache"){
            //
            std::cout <<  resp->command << ": " << resp->message << std::endl;
        }else if(resp->command == "get_active_users"){
            //getActiveUsers(resp->message);
            std::cout <<  resp->command << ": " << resp->message << std::endl;
        }else if (resp->command == "get_messages"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //setMessages(resp->message);
        }else if (resp->command == "close_connections"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //closeConnection();
        }else if (resp->command == "message"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //messageReceived(resp->message, resp->uuid, resp->recipient, resp->recipientName);
        }else if (resp->command == "set_user_cache"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //
        }else if (resp->command == "get_user_info"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //userInfo(resp->message);
        }else if (resp->command == "client_join"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //clientJoin(resp->message);
        }else if(resp->command == "client_leave"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //clientLeave(resp->message);
        }else if(resp->command == "get_user_status"){
            std::cout << resp->command << ": " <<  resp->message << std::endl;
            //requestUserStatus(resp->message);
        }else if(resp->command == "reset_unread_messages"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //
        }else if(resp->command == "get_unread_messages"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //unreadMessages(resp->message);
        }else if(resp->command == "command_to_qt_client"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //
        }else if(resp->command == "get_group_list"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //getGroupList(resp->message);
        }else if(resp->command == "get_users"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //getListUsers(resp->message);
        }else if(resp->command == "add_group"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //addGroupUsers(resp->message);
        }else if(resp->command == "edit_group"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //editGroupUsers(resp->message);
        }else if(resp->command == "remove_group"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //removeGroupUsers(resp->message);
        }else if (resp->command == "add_user"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //addUser(resp->message);
        }else if (resp->command == "remove_user"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //deleteUser(resp->message);
        }else if (resp->command == "update_user"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //updateUser(resp->message);
        }else if (resp->command == "set_parent"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //setUserParent(resp->message);
        }else if(resp->command == "get_channel_token"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //
        }else if (resp->command == "get_webdav_settings"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //
        }else if (resp->command == "set_webdav_settings"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
            //обновились настройки webdav на сервере
        }else if (resp->command == "command_to_qt_agent"){
            std::cout <<  resp->command << ": " << resp->message << std::endl;
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
    client->host = "192.168.43.28";
    client->port = 8080;
    client->app_name = "console";

    client->open();

    std::cout << "exit thread" << std::endl;

}

int main(int argc, char** argv)
{
    bool synch = false;

    setlocale(LC_ALL, "Russian");

    if (!synch){
        _callback_message callback = [](auto && PH1) { return ext_message(std::forward<decltype(PH1)>(PH1)); };
        _callback_status callback_status = [](auto && PH1) { return status_changed(std::forward<decltype(PH1)>(PH1)); };

        client = new IClient("192.168.43.18", 8080, callback, callback_status);
    }else
        client = new IClient();


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
            if (!synch){
                start();
                boost::unique_lock <boost::mutex> lck(mtx);
                cv.wait(lck);
                std::cout << "end lock" << std::endl;
            }
            else{
                bool result = client->synch_session_open("192.168.43.4", "8080");
                client->synch_session_write("ping");
                std::cout << "synch_session_open: " << result << std::endl;
            }

            continue;
        }
        else if (line == "stop")
        {
            if (!synch)
                client->close();
            else
                client->synch_session_close();
            continue;
        }
        else if (line == "exit")
        {
            if (!synch){
                if (client->started())
                    client->close();
            }else
            {
                if(client->synch_session_is_open())
                    client->synch_session_close();
            }

            break;
        }
        else if (line == "started")
        {
            if (!synch)
                std::cout << "started: " << client->started() << std::endl;
            else
                std::cout << "started: " << client->synch_session_is_open() << std::endl;
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
        }else if(line == "set_param"){
            if (synch){
                bool res = client->synch_session_set_param("admin", "admin");
                std::cout << "set_param: " << res << std::endl;
            }

            continue;
        }else if(line == "get_buffer"){
            if (synch){
                std::string resp = client->synch_session_get_buffer();
                if (!resp.empty()){
                    std::cout << arcirk::base64_decode(client->synch_session_get_buffer()) << std::endl;
                }else
                    std::cout << client->synch_session_get_buffer() << std::endl;
            }

            continue;
        }else if(line == "read"){
            if (synch){
                client->synch_session_read();
                std::cout << client->synch_session_get_buffer() << std::endl;
            }
            continue;
        }

        else
        {
            if (synch)
                client->synch_session_write(line);
//            if (client->started()){
//                client->se
//            }
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


