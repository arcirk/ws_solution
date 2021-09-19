//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "./include/shared_state.hpp"

#include <utility>
#include "./include/websocket_session.hpp"


//#include <boost/regex.hpp>

shared_state::
shared_state(std::string doc_root)
        : doc_root_(std::move(doc_root))
{
    sqlite3Db = new arc_sqlite::sqlite3_db();
    sqlite3Db->open("../app/base/db.sqlite");
    sqlite3Db->check_database_table(arc_sqlite::eUsers);
    sqlite3Db->check_database_table(arc_sqlite::eMessages);
    sqlite3Db->check_database_table(arc_sqlite::eChannels);
    sqlite3Db->check_database_table(arc_sqlite::eSubscribers);

    //ищем пользователя с правами администратора
    std::string query = "SELECT * FROM Users WHERE role = 'admin'";
    int result = sqlite3Db->exec(query);

    if (result == 0){
        _add_new_user("admin", "admin", "admin", "", "admin");


//        INSERT INTO Users (
//                FirstField,
//                SecondField,
//                Ref,
//                hash,
//                role,
//                Performance
//                )
//                VALUES (
//                        'admin',
//                        'admin',
//                        'e89dd4ff-e25a-4c9d-9611-85a7c379eda7',
//                        'hash',
//                        'admin',
//                        'admin'
//                        );

    }

}

void shared_state::_add_new_user(const std::string &usr, const std::string &pwd, const std::string &role,
                                 const std::string &uuid, const std::string &perf, const std::string &parent, std::string& error, bool pwd_is_hash) {

    std::string  hash;
    if (!pwd_is_hash)
        hash = arc_json::get_hash(usr, pwd);
    else
        hash = pwd;

    boost::uuids::uuid _uuid{};
    if (uuid.empty()){
        _uuid = boost::uuids::random_generator()();
    } else
        _uuid = arc_json::string_to_uuid(uuid);

    std::vector<arc_json::content_value> fields;

    std::string _parent = parent;
    if (_parent.empty())
        _parent = arc_json::nil_uuid();

    fields.emplace_back(arc_json::content_value("FirstField", usr));
    fields.emplace_back(arc_json::content_value("SecondField", perf));
    fields.emplace_back(arc_json::content_value("Ref", arc_json::uuid_to_string(_uuid)));
    fields.emplace_back(arc_json::content_value("hash", hash));
    fields.emplace_back(arc_json::content_value("role", role));
    fields.emplace_back(arc_json::content_value("channel", _parent));

    sqlite3Db->incert(arc_sqlite::tables::eUsers, fields, error);

    if (error.empty())
        std::cout << "Регистрация пользователя - "<< usr << ":" << hash << std::endl;
}

void
shared_state::
join(websocket_session* session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    //sessions_.insert(session);
    sessions_.insert(std::pair<boost::uuids::uuid, websocket_session*>(session->get_uuid(), session));
}

void
shared_state::
leave(websocket_session* session)
{
    std::cout << "client leave: " << session->get_name() << std::endl;

    if (session->authorized){
        // Оповещаем всех об отключении клиента
        send(std::string ("Отключился клиент: ") + session->get_name() + ":" + arc_json::uuid_to_string(session->get_uuid()), "client_leave");
    }

    std::lock_guard<std::mutex> lock(mutex_);

    auto iter_u = user_sessions.find(session->get_user_uuid());
    if (iter_u != user_sessions.end() ){
        if (iter_u->second.size() <= 1){
            user_sessions.erase(session->get_user_uuid());
            std::cout << "erase user sessions" << std::endl;
        }
        else{
            iter_u->second.erase(find(iter_u->second.begin(), iter_u->second.end(),session));
            std::cout << "close user session" << std::endl;
        }
    }

    auto iter = sessions_.find(session->get_uuid());
    if (iter != sessions_.end() ){
        sessions_.erase(session->get_uuid());
    }

}

void
shared_state::deliver(const std::string& message, websocket_session *session)
{
    std::string result;
    std::string command("message");
    std::string uuid_form;

    //это команда: cmd base64
    if (is_cmd(message))
    {
        std::cout << "deliver: Передана команда серверу" << std::endl;
        std::string res;
        std::string msg_ = run_cmd(message, session->get_uuid(), command, uuid_form, res);

        //если установка параметров сессии, требуется обновить параметры на клиенте
        if (command == "set_client_param")
            result.append("result ");

        boost::uuids::uuid uuid_channel{};
        auto * msg = new arc_json::ws_message(session->get_uuid(), session->get_name(), msg_,
                                              command, session->get_app_name(),
                                              uuid_channel,res,"", session->get_user_uuid(), uuid_form);
        result.append(msg->get_json(true));

        delete msg;
    }else
    {
        result = message;
    }

    if (result == "\n")
        return;

    if (result == "ping"){
        result = "pong";
    } else{
        //отправка сообщения доступна только авторизованным пользователям
        try {
            session->throw_authorized();
        }catch (boost::exception const &e){
            std::cerr << boost::diagnostic_information( e ) << std::endl;
            if (command != "set_client_param"){
                auto * msg = new arc_json::ws_message(session->get_uuid(),
                                                      session->get_name(),
                                                      "Отказано в доступе!",
                                                      command,
                                                      session->get_app_name(),
                                                      boost::uuids::nil_uuid(),
                                                      "error",
                                                      "", session->get_user_uuid(),
                                                      uuid_form);
                result = msg->get_json(true);
                delete msg;
            }
            auto const ss = boost::make_shared<std::string const>(std::move(result));
            session->send(ss);
            return;
        }
    }

    //на всякий случай
    if (result.empty())
        return;

    //это сообщение: msg uuid_получателя base64
    if (is_msg(message)){

        boost::uuids::uuid uuid_recipient{}; //uuid пользователя (не сессии)
        std::string err;
        std::string base64;
        if (is_valid_message(message, uuid_recipient, base64, err)){
            send_message(base64, uuid_recipient, session);
        }else{
            auto * msg = new arc_json::ws_message(session->get_uuid(),
                                                  session->get_name(),
                                                  err,
                                                  command,
                                                  session->get_app_name(),
                                                  boost::uuids::nil_uuid(),
                                                  "error",
                                                  "", session->get_user_uuid(),
                                                  uuid_form);
            result = msg->get_json(true);
            auto const ss = boost::make_shared<std::string const>(std::move(result));
            session->send(ss);
            delete msg;
        }

    }else{
        auto const ss = boost::make_shared<std::string const>(std::move(result));

        //int i = session->get_subscribers()->size();

        for(auto const& wp : *session->get_subscribers())
            wp.second->deliver(ss);
    }



}

// Рассылка сообщения на все клиентские сеансы websocket
// uuid - отправитель сообщения
void
shared_state::
send(const std::string& message, std::string command, bool to_base64)
{

    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p.second->weak_from_this());
    }

    for(auto const& wp : v)
        if(auto sp = wp.lock()){
            if (to_base64){
                auto * new_msg = new arc_json::ws_message(sp->get_uuid(), sp->get_name(), message, command, sp->get_app_name());
                std::string sz_new_msg = new_msg->get_json(true);
                auto const ss = boost::make_shared<std::string const>(std::move(sz_new_msg));
                sp->send(ss);
                delete new_msg;
            } else
            {
                auto const ss = boost::make_shared<std::string const>(std::move(message));
                sp->send(ss);
            }

        }

}

bool shared_state::is_cmd(const std::string& msg)
{
    std::string cmd = msg.substr(0, 3);

    if (cmd == "cmd")
    {
        return true;
    }

    return false;
}

std::string
shared_state::run_cmd(const std::string &cmd, boost::uuids::uuid &uuid, std::string &command, std::string &uuid_form,
                      std::string &res)
{

    //формат сообщения
    //cmd base64

    //делим строку, разделитель пробел

    std::string message = cmd;

    if (cmd.substr(cmd.length() - 1, cmd.length()) == "\n"){
        message = cmd.substr(0, cmd.length()  - 1) ;
    }

    arc_json::T_vec v = arc_json::split(std::move(message), " ");

    std::string result = "unknown command!";

    command = "unknown command!";

    if (v.size() != 2){
        res = "error";
        return "request format error!";
    }


    std::string base64 = v[1];
    std::string json;

    //декодируем 2 элемент массива
    try {
        json = arc_json::base64_decode(base64);
    }catch (std::exception& e){
        res = "error";
        return std::string ("error: ") + e.what() ;
    }

    // парсим
    auto * msg = new arc_json::ws_message(json);

    if (msg->is_parse) {

//        //возможно команда имеет формат
//        //command uuid "message"
//        //поэтому с начала удаляем строки заключенные в кавычки
//        std::string sz_command = arc_json::remove_text_enclosed_in_quotation_marks(msg->get_message());
//        //Получаем строку заключенную в кавычки в массив т.к. таких строк может быть несколько
//        arc_json::T_vec vec_message = arc_json::get_text_in_quotation_marks(msg->get_message());
//        //получаем массив значений команды в формате command arg1 arg2 ...argn
//        arc_json::T_vec vec_command = arc_json::split(sz_command, " ");
//
//        command = vec_command[0];

        command = msg->get_command();

        std::cout << "run command: " << command << std::endl;

        uuid_form = msg->get_uuid_form();

        if (!is_valid_command_name(command)){
            res = "error";
            return "unknown command!";
        }

        auto * params = new arc_json::ws_json();
        std::string tmp_msg = msg->get_message();
        params->parse(tmp_msg);

        if (!params->is_parse()){
            res = "error";
            return "error parse parameters command!";
        }

        if (!is_valid_param_count(command, params->get_member_count())){
            res = "error";
            return "wrong number of parameters!";
        }

        std::string err = "error";
        std::string custom_result = "";

        bool _res = get_cmd_func(command)(uuid, params, msg, err, custom_result);
        if (_res){
            if (custom_result.empty())
                result = command + ": success";
            else
                result = custom_result;
            res = "success";
        } else{
            result = command + ": " + err;
            res = "error";
        }

        std::cout << "end command: " << command << std::endl;

        delete params;
    }

    delete msg;

    return result;
}

//получение непосредственно сессии
websocket_session*
shared_state::get_session(boost::uuids::uuid &uuid) {
    auto iter = sessions_.find(uuid);
    if (iter != sessions_.end() ){
        return iter->second;
    }
    return nullptr;
}

//получение всех сессий пользователя
std::vector<websocket_session *>
shared_state::get_sessions(boost::uuids::uuid &user_uuid) {
    auto iter = user_sessions.find(user_uuid);
    if (iter != user_sessions.end() ){
        return iter->second;
    }
    return {};
}

void shared_state::send_private_message(const std::string &message, boost::uuids::uuid &recipient,
                                        boost::uuids::uuid &sender) {

//    auto session = get_session(recipient);
//    auto session_sender = get_session(sender);
//
//    std::string name = "admin";
//
//    if (session_sender){
//        name = session_sender->get_name();
//    }
//
//    if (session){
//
//        auto* msg = new arc_json::ws_message(session->get_uuid(), name, message, "send_private_message", session->get_app_name());
//
//        auto const ss = boost::make_shared<std::string const>(msg->get_json(true));
//
//        session->send(ss);
//
//        delete msg;
//    }

}
void shared_state::on_start() {

    //get shared channels
    //run_session_shared_channels();
}


bool shared_state::is_valid_param_count(const std::string &command, unsigned int params) {
    if (command == "set_client_param")
        return params == 5;
    else if (command == "get_active_users")
        return params == 1;
    else if (command == "send_all_message")
        return params == 2;
    else if (command == "subscribe_to_channel")
        return params == 3;
    else if (command == "subscribe_exit_channel")
        return params == 3;
    else if (command == "add_user")
        return params == 6;
    else if (command == "get_users")
        return params == 1;
    else if (command == "update_user")
        return params == 2;
    else if (command == "get_messages")
        return params == 4;
    else if (command == "get_user_info")
        return params == 1;
    else if (command == "get_group_list")
        return params == 1;
    else if (command == "add_group")
        return params == 3;
    else if (command == "edit_group")
        return params == 4;
    else if (command == "remove_group")
        return params == 1;
    else if (command == "set_parent")
        return params == 2;
    else if (command == "remove_user")
        return params == 1;
    else
        return false;
}

bool shared_state::is_valid_command_name(const std::string &command) {
    std::vector<std::string> commands;

    commands.emplace_back("set_client_param");
    commands.emplace_back("get_active_users");
    commands.emplace_back("send_all_message");
    commands.emplace_back("get_uuid");
    commands.emplace_back("add_user");
    commands.emplace_back("subscribe_to_channel");
    commands.emplace_back("subscribe_exit_channel");
    commands.emplace_back("send_private_message");
    commands.emplace_back("message");
    commands.emplace_back("get_users");
    commands.emplace_back("update_user");
    commands.emplace_back("get_messages");
    commands.emplace_back("get_user_info");
    commands.emplace_back("get_group_list");
    commands.emplace_back("add_group");
    commands.emplace_back("edit_group");
    commands.emplace_back("remove_group");
    commands.emplace_back("set_parent");
    commands.emplace_back("remove_user");

    return std::find(commands.begin(), commands.end(), command) != commands.end();
}

cmd_func shared_state::get_cmd_func(const std::string& command) {
    if (command == "set_client_param")
        return  std::bind(&shared_state::set_client_param, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_active_users")
        return  std::bind(&shared_state::get_active_users, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "send_all_message")
        return  std::bind(&shared_state::send_all_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "subscribe_to_channel")
        return  std::bind(&shared_state::subscribe_to_channel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "subscribe_exit_channel")
        return  std::bind(&shared_state::subscribe_exit_channel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "add_user")
        return  std::bind(&shared_state::add_new_user, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_users")
        return  std::bind(&shared_state::get_db_users, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "update_user")
        return  std::bind(&shared_state::update_user, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_messages")
        return  std::bind(&shared_state::get_messages, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_user_info")
        return  std::bind(&shared_state::get_user_info, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_group_list")
        return  std::bind(&shared_state::get_group_list, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "add_group")
        return  std::bind(&shared_state::add_group, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "edit_group")
        return  std::bind(&shared_state::edit_group, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "remove_group")
        return  std::bind(&shared_state::remove_group, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "set_parent")
        return  std::bind(&shared_state::set_parent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "remove_user")
        return  std::bind(&shared_state::remove_user, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else
        return nullptr;
}

bool
shared_state::set_client_param(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result){

    //Установка основных параметров сессии + авторизация
    try {

        boost::uuids::uuid new_uuid = arc_json::string_to_uuid(params->getStringMember("uuid"), true);
        std::string name = params->getStringMember("name");
        std::string pwd = params->getStringMember("pwd");
        std::string hash = arc_json::get_hash(name, pwd);
        std::string app_name = params->getStringMember("app_name");
        boost::uuids::uuid user_uuid = arc_json::string_to_uuid(params->getStringMember("user_uuid"), true);


        if (get_session(new_uuid)){
            err = "Клиент с идентификатором '" + params->getStringMember("uuid") + "' уже зарегистрирован!";
            return false;
        }

        websocket_session* session = get_session(uuid);

        if (!session){
            err = "Требуемая сессия не найдена!";
            return false;
        }

        session->set_name(name);

        std::string role = "client";

        if (!session->authorized){

            if (!hash.empty()){
                std::cout << session->get_name() << ":" << hash << std::endl;
                //int result = sqlite3Db->exec("select _id from Users where hash = '" + hash + "';");
                std::vector<std::map<std::string, std::string>> table;
                err = "";
                int result = sqlite3Db->execute("select _id, role from Users where hash = '" + hash + "';",
                                                "Users", table, err);
                session->authorized = result > 0;
                if (result > 0){
                    role = table[0].at("role");
                }
            }

        }

        if (session->authorized){

            sessions_.erase(uuid);
            session->set_uuid(new_uuid);
            session->set_user_uuid(user_uuid);
            session->set_app_name(app_name);
            session->set_role(role);
            sessions_.insert(std::pair<boost::uuids::uuid, websocket_session*>(session->get_uuid(), session));
            // Оповещаем всех клиентов о регистрации нового клиент
            send(std::string ("Подключился новый клиент: ") + session->get_name() + ":" + arc_json::uuid_to_string(session->get_uuid()), "set_client_param");
            std::cout << "Успешная аутентификация пользователя: " + session->get_name() << std::endl;

            //Добавляем в список собственных сессий пользователя
            auto iter = user_sessions.find(session->get_user_uuid());
            if (iter == user_sessions.end() ){
                std::vector<websocket_session*> vec;
                vec.push_back(session);
                user_sessions.insert(std::pair<boost::uuids::uuid, std::vector<websocket_session*>>(session->get_user_uuid(), vec));
                std::cout << "Добавляем в список собственных сессий" << std::endl;
            }else{
                iter->second.push_back(session);
                std::cout << "Обновляем в список собственных сессий" << std::endl;
            }

            return true;
        }else
            err = "Отказано в доступе";

        return false;

    } catch(std::exception& e){
        err = e.what();
        return false;
    }
}

bool shared_state::get_active_users(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {

    arc_json::ws_json json_arr{};
    json_arr.set_array();

    for (auto p : sessions_){
        _Value doc(rapidjson::Type::kObjectType);
        json_arr.addObjectMember(&doc, arc_json::content_value("uuid", to_string(p.second->get_uuid())));
        json_arr.addObjectMember(&doc, arc_json::content_value("name", p.second->get_name()));
        json_arr.addObjectMember(&doc, arc_json::content_value("user_uuid", to_string(p.second->get_user_uuid())));
        json_arr.addObjectMember(&doc, arc_json::content_value("app_name", p.second->get_app_name()));
        json_arr.push_back(doc) ;
    }

    custom_result = json_arr.to_string();

    return true;

}

bool shared_state::send_all_message(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {

    std::string message = params->getStringMember("message");

    if (message.empty()){
        err = "Передано пустое сообщение!";
        return false;
    }

    auto * new_msg = new arc_json::ws_message(uuid, msg->get_name(), message, "send_all_message", msg->get_app_name());
    std::string sz_new_msg = new_msg->get_json(true);
    send(sz_new_msg, "message", false);
    delete new_msg;
    return true;

}

bool shared_state::is_msg(const std::string& msg)
{
    std::string cmd = msg.substr(0, 3);

    if (cmd == "msg")
    {
        return true;
    }

    return false;
}

void shared_state::send_message(const std::string &message, boost::uuids::uuid &recipient,
                                websocket_session *current_sess) {

    if (recipient == boost::uuids::nil_uuid()){
        //отправить всем (общий чат)
        send(message, "message", false);
    } else
    {
        //отправляем сообщение во все сессии получателя
        //recipient - идентификатор пользователя
        //найдем все сессии с этим идентификатором
        //также дублируем сообщение во все текущие сессии

        std::vector<boost::weak_ptr<websocket_session>> v;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            v.reserve(get_sessions(recipient).size() + get_sessions(current_sess->get_user_uuid()).size());
            for(auto p : get_sessions(recipient))
                v.emplace_back(p->weak_from_this());
            for(auto p : get_sessions(current_sess->get_user_uuid()))
                v.emplace_back(p->weak_from_this());
        }

        auto const ss = boost::make_shared<std::string const>(std::move(message));

        for(auto const& wp : v)
            if(auto sp = wp.lock()){
                sp->send(ss);
            }

        sqlite3Db->save_message(message, current_sess->get_user_uuid(), recipient);

    }

}

bool shared_state::is_valid_message(const std::string &message, boost::uuids::uuid &uuid_recipient, std::string &base64,
                                    std::string &err) {

    std::string _message = message;

    if (message.substr(message.length() - 1, message.length()) == "\n"){
        _message = message.substr(0, message.length()  - 1) ;
    }

    arc_json::T_vec v = arc_json::split(std::move(_message), " ");


    if (v.size() != 3){
        err = "request format error!";
        return false;
    }

    std::string _uuid_recipient = v[1];
    base64 = v[2];

    if (_uuid_recipient != "00000000-0000-0000-0000-000000000000"){
        if (!arc_json::is_valid_uuid(_uuid_recipient, uuid_recipient)){
            err = "error uuid recipient!";
            return false;
        }
    } else
        uuid_recipient = boost::uuids::nil_uuid();

    return true;
}

bool shared_state::subscribe_to_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {

    //params[1] - идентификатор формы
    boost::uuids::uuid sub = arc_json::string_to_uuid(params->getStringMember("uuid_channel"));

    auto session_channel = get_session(sub);
    auto session = get_session(uuid);

    if (session && session_channel){
        session_channel->join_channel(session);
        std::cout << "subscribe_to_channel:ok" << std::endl;
        send_private_message("subscribe_to_channel:true", sub, uuid);
        return true;
    }
    std::cout << "subscribe_to_channel:error" << std::endl;
    return false;

}

bool shared_state::subscribe_exit_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {

    auto session_channel = get_session(msg->get_uuid_channel());

    if (session_channel)
        session_channel->close_channel(msg->get_uuid());

    return true;
}

bool shared_state::add_new_user(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message *msg,
                                std::string &err, std::string &custom_result) {

    err = "";

    _add_new_user(
            params->getStringMember("user"), //user
            params->getStringMember("pwd"), //pwd
            params->getStringMember("role"), //role
            params->getStringMember("uuid"), //uuid
            params->getStringMember("perf"), //perf
            params->getStringMember("parent"), //perf
            err,
            false);
    if (!err.empty())
        return false;
    else
        return true;
}

bool shared_state::get_db_users(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message *msg,
                                std::string &err, std::string &custom_result) {

    std::string channel = params->getStringMember("channel");

    std::string query = "SELECT * FROM Users";

    if (!channel.empty()){
        query += " WHERE channel = '" + channel + "'";
    }

    err = "";
    sqlite3Db->execute(query, "Users", custom_result, err);

    if (!err.empty()){
        return false;
    }

    return true;
}

bool shared_state::update_user(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message *msg,
                               std::string &err, std::string &custom_result) {

    std::string query = "UPDATE Users SET ";

    try {
        std::vector<arc_json::content_value> m_sets;
        if (!params->getMembers("set", m_sets)) {
            err = "error parse parameters command";
            return false;
        }
        std::vector<arc_json::content_value> m_where;
        if (!params->getMembers("where", m_where)) {
            err = "error parse parameters command";
            return false;
        }

        for (auto itr = m_sets.begin(); itr < m_sets.end(); itr++){
            std::string tmp = itr->key;
            tmp.append(" = '");
            tmp.append(boost::get<std::string>(itr->value));
            tmp.append("'");
            query.append(" " + tmp);
            if (itr != --m_sets.end())
                query.append(",");

        }

        query.append(" WHERE ");

        for (auto itr = m_where.begin(); itr < m_where.end(); itr++){
            std::string tmp = itr->key;
            tmp.append(" = '");
            tmp.append(boost::get<std::string>(itr->value));
            tmp.append("'");
            query.append(" " + tmp);
            if (itr != --m_where.end())
                query.append(" AND ");
        }

    }catch (std::exception& e){
        err = e.what();
        return false;
    }

    err = "";
    sqlite3Db->exec(query, err);

    if (err.empty())
        return true;
    else
        return false;
}

bool shared_state::get_messages(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                                std::string &err, std::string &custom_result) {

    boost::uuids::uuid recipient{};

    if (!arc_json::is_valid_uuid(params->getStringMember("recipient"), recipient)){
        err = "Не действительный идентификатор подписчика!";
        return false;
    }

    std::string token = sqlite3Db->get_channel_token(get_session(uuid)->get_user_uuid(), recipient);

    if (token.empty() || token == "error"){
        err = "Ошибка получения токена сообщений!";
        return false;
    }

    int limit = 0;
    params->getMember("limit", limit);
    int start_date = 0;
    params->getMember("start_date", start_date);
    int end_date = 0;
    params->getMember("end_date", end_date);

    std::string json;

    err = "";

    int result = sqlite3Db->get_save_messages(json, token, err, limit, start_date, end_date);

    if (result > 0){
        custom_result = json;
    } else
        custom_result = "{}";

    if (!err.empty())
        return false;

    return true;

}

bool shared_state::get_user_info(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                                 std::string &err, std::string &custom_result) {


    std::string user_uuid = params->getStringMember("user_uuid");

    if (!user_uuid.empty() && user_uuid != arc_json::nil_uuid()){

        std::string query = "select * from Users where Ref = '" + user_uuid + "';";
        std::vector<std::map<std::string , std::string>> row;
        std::string err = "";

        auto json = new arc_json::ws_json();
        json->set_object();

        int result = sqlite3Db->execute(query, "Users", row, err);

        if (result > 0){
            json->addMember(arc_json::content_value("name", row[0].at("FirstField")));
            json->addMember(arc_json::content_value("performance", row[0].at("SecondField")));
            json->addMember(arc_json::content_value("user_uuid", row[0].at("Ref")));
        } else
            json->addMember(arc_json::content_value("name", "не зарегистрирован"));

        websocket_session* session = nullptr;
        boost::uuids::uuid user_uuid_ = arc_json::string_to_uuid(user_uuid);

        auto sessions = get_sessions(user_uuid_);

        if (sessions.size() > 0){
            json->addMember(arc_json::content_value("active", true));
        } else
            json->addMember(arc_json::content_value("active", false));

        custom_result = json->to_string();
        delete json;
    } else{
        err = "Ошибка получения данных пользователя!";
        return false;
    }

    return true;
}

bool shared_state::get_group_list(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                                  std::string &err, std::string &custom_result) {
    std::string query = "SELECT * FROM Channels";
    err = "";
    sqlite3Db->execute(query, "Channels", custom_result, err);

    if (!err.empty()){
        return false;
    }

    return true;
}

bool shared_state::add_group(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                             std::string &err, std::string &custom_result) {

    auto  current_sess = get_session(uuid);

    try {
        current_sess->throw_authorized();
    }catch (boost::exception const &e) {
        err = boost::diagnostic_information(e);
        std::cerr << err << std::endl;
        return false;

    }

    if (current_sess->get_role() != "admin"){
        err = "не достаточно прав доступа для команды!";
        return false;
    }

    std::string group_name = params->getStringMember("name");
    std::string group_presentation = params->getStringMember("presentation");
    std::string group_parent = params->getStringMember("parent");
    boost::uuids::uuid _uuid = boost::uuids::random_generator()();

    std::string _group_parent;
    if (group_parent.empty()){
        _group_parent = arc_json::nil_uuid();
    } else
        _group_parent = group_parent;

    std::vector<arc_json::content_value> fields;

    fields.emplace_back(arc_json::content_value("FirstField", group_name));
    fields.emplace_back(arc_json::content_value("SecondField", group_presentation));
    fields.emplace_back(arc_json::content_value("Ref", arc_json::uuid_to_string(_uuid)));
    fields.emplace_back(arc_json::content_value("Parent", _group_parent));

    err = "";
    sqlite3Db->incert(arc_sqlite::tables::eChannels, fields, err);

    if (!err.empty())
        return false;
    else{
        auto * obj = new arc_json::ws_json();
        obj->set_object();
        obj->addObject(fields);
        custom_result = obj->to_string();
        delete obj;
        return true;
    }

}
bool shared_state::edit_group(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                              std::string &err, std::string &custom_result) {
    auto  current_sess = get_session(uuid);

    try {
        current_sess->throw_authorized();
    }catch (boost::exception const &e) {
        err = boost::diagnostic_information(e);
        std::cerr << err << std::endl;
        return false;

    }

    if (current_sess->get_role() != "admin"){
        err = "не достаточно прав доступа для команды!";
        return false;
    }

    std::string group_name = params->getStringMember("name");
    std::string group_presentation = params->getStringMember("presentation");
    std::string group_parent = params->getStringMember("parent");
    std::string _uuid = params->getStringMember("ref");

    if (_uuid.empty()){
        err = "не указан идентификатор группы!";
        return false;
    }

    std::vector<arc_json::content_value> _sets;

    _sets.emplace_back(arc_json::content_value("FirstField", group_name));
    _sets.emplace_back(arc_json::content_value("SecondField", group_presentation));
    _sets.emplace_back(arc_json::content_value("Parent", group_parent));

    std::vector<arc_json::content_value> _where;
    _where.emplace_back(arc_json::content_value("Ref", _uuid));

    err = "";
    sqlite3Db->update(arc_sqlite::tables::eChannels, _sets, _where, err);

    if (!err.empty())
        return false;
    else{
        _sets.emplace_back(arc_json::content_value("Ref", _uuid));
        auto * obj = new arc_json::ws_json();
        obj->set_object();
        obj->addObject(_sets);
        custom_result = obj->to_string();
        delete obj;
        return true;
    }
}

bool shared_state::remove_group(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                                std::string &err, std::string &custom_result) {
    auto  current_sess = get_session(uuid);

    try {
        current_sess->throw_authorized();
    }catch (boost::exception const &e) {
        err = boost::diagnostic_information(e);
        std::cerr << err << std::endl;
        return false;

    }

    if (current_sess->get_role() != "admin"){
        err = "не достаточно прав доступа для команды!";
        return false;
    }

    std::string _uuid = params->getStringMember("ref");
    if (_uuid.empty()){
        err = "не указан идентификатор группы!";
        return false;
    }

    std::string query = "DELETE FROM Channels\n"
                        "WHERE Ref = '" + _uuid + "' OR\n"
                        "Parent = '" + _uuid + "';";

    err = "";
    sqlite3Db->exec(query, err);
    if(!err.empty())
        return false;

    custom_result = params->to_string();

    return true;
}

bool shared_state::set_parent(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                              std::string &err, std::string &custom_result) {

    auto  current_sess = get_session(uuid);

    try {
        current_sess->throw_authorized();
    }catch (boost::exception const &e) {
        err = boost::diagnostic_information(e);
        std::cerr << err << std::endl;
        return false;

    }

    if (current_sess->get_role() != "admin"){
        err = "не достаточно прав доступа для команды!";
        return false;
    }
    std::string _uuid = params->getStringMember("user");
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!";
        return false;
    }

    std::string _parent = params->getStringMember("parent");
    if (_uuid.empty()){
        _parent = arc_json::nil_uuid();
    }

    std::string query = "UPDATE Users\n"
                        " SET channel = '" + _parent + "'\n"
                        " WHERE Ref = '" + _uuid + "';";

    err = "";
    sqlite3Db->exec(query, err);
    if(!err.empty())
        return false;

    custom_result = params->to_string();

    return true;
}

bool shared_state::remove_user(boost::uuids::uuid &uuid, arc_json::ws_json *params, arc_json::ws_message *msg,
                               std::string &err, std::string &custom_result) {

    auto  current_sess = get_session(uuid);

    try {
        current_sess->throw_authorized();
    }catch (boost::exception const &e) {
        err = boost::diagnostic_information(e);
        std::cerr << err << std::endl;
        return false;

    }

    if (current_sess->get_role() != "admin"){
        err = "не достаточно прав доступа для команды!";
        return false;
    }

    std::string _uuid = params->getStringMember("ref");
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!!";
        return false;
    }

    std::string query = "DELETE FROM Users\n"
                        "WHERE Ref = '" + _uuid + "';";

    err = "";
    sqlite3Db->exec(query, err);
    if(!err.empty())
        return false;

    custom_result = params->to_string();

    return true;
}
