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
    }
    //справочник пользователей
    query = "CREATE VIEW IF NOT EXISTS UsersCatalog AS "
            "    SELECT FirstField, "
            "           SecondField, "
            "           Ref, "
            "           Parent, "
            "           1 AS IsGroup "
            "      FROM Channels "
            "    UNION "
            "    SELECT FirstField, "
            "           SecondField, "
            "           Ref, "
            "           channel AS Parent, "
            "           0 AS IsGroup "
            "      FROM Users;";

    result = sqlite3Db->exec(query);

}

void shared_state::_add_new_user(const std::string &usr, const std::string &pwd, const std::string &role,
                                 const std::string &uuid, const std::string &perf, const std::string &parent, std::string& error, bool pwd_is_hash) {

    std::string  hash;
    if (!pwd_is_hash)
        hash = arcirk::get_hash(usr, pwd);
    else
        hash = pwd;

    boost::uuids::uuid _uuid{};
//    if (uuid.empty()){
//        _uuid = boost::uuids::random_generator()();
//    } else
        _uuid = arcirk::string_to_uuid(uuid, true);

    std::vector<arcirk::content_value> fields;

    std::string _parent = parent;
    if (_parent.empty())
        _parent = arcirk::nil_string_uuid();

    fields.emplace_back(arcirk::content_value("FirstField", usr));
    fields.emplace_back(arcirk::content_value("SecondField", perf));
    fields.emplace_back(arcirk::content_value("Ref", arcirk::uuid_to_string(_uuid)));
    fields.emplace_back(arcirk::content_value("hash", hash));
    fields.emplace_back(arcirk::content_value("role", role));
    fields.emplace_back(arcirk::content_value("channel", _parent));

    sqlite3Db->insert(arc_sqlite::tables::eUsers, fields, error);

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
        send(std::string ("Отключился клиент: ") + session->get_name() + ":" + arcirk::uuid_to_string(session->get_uuid()), "client_leave");
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

_ws_message shared_state::createMessage(websocket_session *session) {
    _ws_message _message = _ws_message();
    _message.uuid = session->get_uuid();
    _message.name = session->get_name();
    _message.app_name = session->get_name();
    _message.uuid_user = session->get_user_uuid();
    _message.role = session->get_role();
    std::cout << _message.uuid << std::endl;
    return _message;
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

        _ws_message _message = createMessage(session);
        _message.message = msg_;
        _message.command = command;
        _message.result = res;
        _message.uuid_form = arcirk::string_to_uuid(uuid_form, false);

        auto * msg = new ws_message(_message);
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
                _ws_message _message = createMessage(session);
                _message.message = "Отказано в доступе!";
                _message.result = "error";
                _message.uuid_form = arcirk::string_to_uuid(uuid_form, false);

                auto * msg = new ws_message(_message);
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
            _ws_message _message = createMessage(session);
            _message.message = err;
            _message.result = "error";
            _message.uuid_form = arcirk::string_to_uuid(uuid_form, false);
            _message.command = command;

            auto * msg = new ws_message(_message);
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
                _ws_message _message = createMessage(sp.get());
                _message.message = message;
                _message.command = command;
                auto * new_msg = new ws_message(_message);
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

    arcirk::T_vec v = arcirk::split(std::move(message), " ");

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
        json = arcirk::base64_decode(base64);
    }catch (std::exception& e){
        res = "error";
        return std::string ("error: ") + e.what() ;
    }

    // парсим
    auto * msg = new ws_message(json);

    if (msg->is_parse()) {

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

        command = msg->message().command;

        std::cout << "run command: " << command << std::endl;

        uuid_form = arcirk::uuid_to_string(msg->message().uuid_form);

        if (!is_valid_command_name(command)){
            res = "error";
            return "unknown command!";
        }

        auto * params = new arcirk::bJson();
        std::string tmp_msg = msg->message().message;
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
                result = "success";
            else
                result = custom_result;
            res = "success";
        } else{
            result = err;
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

//void shared_state::send_private_message(const std::string &message, boost::uuids::uuid &recipient,
//                                        boost::uuids::uuid &sender) {

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

//}
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
//    else if (command == "subscribe_to_channel")
//        return params == 3;
//    else if (command == "subscribe_exit_channel")
//        return params == 3;
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
    else if (command == "kill_session")
        return params == 1;
//    else if (command == "set_uuid")
//        return params == 2;
    else if (command == "set_app_name")
        return params == 2;
    else if (command == "exec_query")
        return params == 1;
    else if (command == "get_users_catalog")
        return params == 1;
    else
        return false;
}

bool shared_state::is_valid_command_name(const std::string &command) {
    std::vector<std::string> commands;

    commands.emplace_back("set_client_param");
    commands.emplace_back("get_active_users");
    commands.emplace_back("send_all_message");
    commands.emplace_back("add_user");
//    commands.emplace_back("subscribe_to_channel");
//    commands.emplace_back("subscribe_exit_channel");
    //commands.emplace_back("send_private_message");
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
    commands.emplace_back("kill_session");
//    commands.emplace_back("set_uuid");
    commands.emplace_back("set_app_name");
    commands.emplace_back("exec_query");
    commands.emplace_back("get_users_catalog");

    return std::find(commands.begin(), commands.end(), command) != commands.end();
}

cmd_func shared_state::get_cmd_func(const std::string& command) {
    if (command == "set_client_param")
        return  std::bind(&shared_state::set_client_param, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "get_active_users")
        return  std::bind(&shared_state::get_active_users, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "send_all_message")
        return  std::bind(&shared_state::send_all_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
//    else if (command == "subscribe_to_channel")
//        return  std::bind(&shared_state::subscribe_to_channel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
//    else if (command == "subscribe_exit_channel")
//        return  std::bind(&shared_state::subscribe_exit_channel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
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
    else if (command == "kill_session")
        return  std::bind(&shared_state::kill_session, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
//    else if (command == "set_uuid")
//        return  std::bind(&shared_state::set_uuid, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "set_app_name")
        return  std::bind(&shared_state::set_app_name, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else if (command == "exec_query")
        return  std::bind(&shared_state::exec_query, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
   else if (command == "get_users_catalog")
        return  std::bind(&shared_state::get_users_catalog, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    else
        return nullptr;
}

bool
shared_state::set_client_param(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message* msg, std::string& err, std::string& custom_result){

    //Установка основных параметров сессии + авторизация
    try {

        boost::uuids::uuid new_uuid = params->get_member("uuid").get_uuid();
        std::string name = params->get_member("name").get_string();
        std::string pwd = params->get_member("pwd").get_string();
        std::string hash = params->get_member("hash").get_string(); //если указан хеш пароль игронится
        if (hash.empty())
            hash = arcirk::get_hash(name, pwd);

        std::string app_name = params->get_member("app_name").get_string();
        boost::uuids::uuid user_uuid = params->get_member("user_uuid").get_uuid();


        if (get_session(new_uuid)){
            err = "Клиент с идентификатором '" + params->get_member("uuid").get_string() + "' уже зарегистрирован!";
            return false;
        }

        websocket_session* session = get_session(uuid);

        if (!session){
            err = "Требуемая сессия не найдена!";
            return false;
        }

        session->set_name(name);

        std::string role = "user";

        if (!session->authorized){

            if (!hash.empty()){
                std::cout << session->get_name() << ":" << hash << std::endl;
                //int result = sqlite3Db->exec("select _id from Users where hash = '" + hash + "';");
                std::vector<std::map<std::string, std::string>> table;
                err = "";
                int result = sqlite3Db->execute("select _id, Ref, role from Users where hash = '" + hash + "';",
                                                "Users", table, err);
                session->authorized = result > 0;
                if (result > 0){
                    role = table[0].at("role");
                    std::string ref = table[0].at("Ref");
                    if (!ref.empty())
                        user_uuid = arcirk::string_to_uuid(ref, true);
                }
            }

        }

        if (session->authorized){

            sessions_.erase(uuid);

            //session->set_uuid(new_uuid);
            session->set_user_uuid(user_uuid);
            session->set_app_name(app_name);
            session->set_role(role);

            sessions_.insert(std::pair<boost::uuids::uuid, websocket_session*>(session->get_uuid(), session));
            // Оповещаем всех клиентов о регистрации нового клиент
            send(std::string ("Подключился новый клиент: ") + session->get_name() + ":" + arcirk::uuid_to_string(session->get_uuid()), "client_join");
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

bool shared_state::get_active_users(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message* msg, std::string& err, std::string& custom_result) {

    arcirk::bJson json_arr{};
    json_arr.set_array();

    for (auto p : sessions_){
        _Value doc(rapidjson::Type::kObjectType);
        json_arr.addMember(&doc, arcirk::content_value("uuid", to_string(p.second->get_uuid())));
        json_arr.addMember(&doc, arcirk::content_value("name", p.second->get_name()));
        json_arr.addMember(&doc, arcirk::content_value("user_uuid", to_string(p.second->get_user_uuid())));
        json_arr.addMember(&doc, arcirk::content_value("app_name", p.second->get_app_name()));
        json_arr.push_back(doc) ;
    }

    custom_result = json_arr.to_string();

    return true;

}

bool shared_state::send_all_message(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message* msg, std::string& err, std::string& custom_result) {

    std::string message = params->get_member("message").get_string();

    if (message.empty()){
        err = "Передано пустое сообщение!";
        return false;
    }

    _ws_message _message = _ws_message();
    _message.uuid = uuid;
    _message.name = msg->message().message;
    _message.message = message;
    _message.command = "send_all_message";
    _message.app_name = msg->message().app_name;

    auto * new_msg = new ws_message(_message);
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

    arcirk::T_vec v = arcirk::split(std::move(_message), " ");


    if (v.size() != 3){
        err = "request format error!";
        return false;
    }

    std::string _uuid_recipient = v[1];
    base64 = v[2];

    if (_uuid_recipient != "00000000-0000-0000-0000-000000000000"){
        if (!arcirk::is_valid_uuid(_uuid_recipient, uuid_recipient)){
            err = "error uuid recipient!";
            return false;
        }
    } else
        uuid_recipient = boost::uuids::nil_uuid();

    return true;
}

//bool shared_state::subscribe_to_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {
//
//    //params[1] - идентификатор формы
//    boost::uuids::uuid sub = arc_json::string_to_uuid(params->getStringMember("uuid_channel"));
//
//    auto session_channel = get_session(sub);
//    auto session = get_session(uuid);
//
//    if (session && session_channel){
//        session_channel->join_channel(session);
//        std::cout << "subscribe_to_channel:ok" << std::endl;
//        send_private_message("subscribe_to_channel:true", sub, uuid);
//        return true;
//    }
//    std::cout << "subscribe_to_channel:error" << std::endl;
//    return false;
//
//}

//bool shared_state::subscribe_exit_channel(boost::uuids::uuid &uuid, arc_json::ws_json* params, arc_json::ws_message* msg, std::string& err, std::string& custom_result) {
//
//    auto session_channel = get_session(msg->get_uuid_channel());
//
//    if (session_channel)
//        session_channel->close_channel(msg->get_uuid());
//
//    return true;
//}

bool shared_state::add_new_user(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                std::string &err, std::string &custom_result) {

    err = "";

    _add_new_user(
            params->get_member("user").get_string(), //user
            params->get_member("pwd").get_string(), //pwd
            params->get_member("role").get_string(), //role
            params->get_member("uuid").to_string(), //uuid
            params->get_member("presentation").get_string(), //presentation
            params->get_member("parent").get_string(), //
            err,
            false);
    if (!err.empty())
        return false;
    else
        return true;
}

bool shared_state::get_db_users(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                std::string &err, std::string &custom_result) {

    std::string channel = params->get_member("channel").get_string();

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

bool shared_state::update_user(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                               std::string &err, std::string &custom_result) {

    std::string query = "UPDATE Users SET ";

    try {
        std::vector<arcirk::content_value> m_sets;
        if (!params->getMembers("set", m_sets)) {
            err = "error parse parameters command";
            return false;
        }
        std::vector<arcirk::content_value> m_where;
        if (!params->getMembers("where", m_where)) {
            err = "error parse parameters command";
            return false;
        }

        for (auto itr = m_sets.begin(); itr < m_sets.end(); itr++){
            std::string tmp = itr->key;
            tmp.append(" = '");
            tmp.append(itr->value.get_string());
            tmp.append("'");
            query.append(" " + tmp);
            if (itr != --m_sets.end())
                query.append(",");

        }

        query.append(" WHERE ");

        for (auto itr = m_where.begin(); itr < m_where.end(); itr++){
            std::string tmp = itr->key;
            tmp.append(" = '");
            tmp.append(itr->value.get_string());
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

bool shared_state::get_messages(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                std::string &err, std::string &custom_result) {

    boost::uuids::uuid recipient{};

    if (!arcirk::is_valid_uuid(params->get_member("recipient").to_string(), recipient)){
        err = "Не действительный идентификатор подписчика!";
        return false;
    }

    std::string token = sqlite3Db->get_channel_token(get_session(uuid)->get_user_uuid(), recipient);

    if (token.empty() || token == "error"){
        err = "Ошибка получения токена сообщений!";
        return false;
    }

    int limit = params->get_member("limit").get_int();
    int start_date = params->get_member("start_date").get_int();
    int end_date = params->get_member("end_date").get_int();


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

bool shared_state::get_user_info(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                 std::string &err, std::string &custom_result) {


    boost::uuids::uuid user_uuid = params->get_member("uuid_user").get_uuid();

    if (user_uuid != arcirk::nil_uuid()){

        std::string query = "select * from Users where Ref = '" + to_string(user_uuid) + "';";
        std::vector<std::map<std::string , std::string>> row;
        std::string err = "";

        auto json = new arcirk::bJson();
        json->set_object();

        int result = sqlite3Db->execute(query, "Users", row, err);

        if (result > 0){
            json->addMember(arcirk::content_value("name", row[0].at("FirstField")));
            json->addMember(arcirk::content_value("performance", row[0].at("SecondField")));
            json->addMember(arcirk::content_value("uuid", row[0].at("Ref")));
            json->addMember(arcirk::content_value("cache", row[0].at("cache")));
            json->addMember(arcirk::content_value("subdivision", get_user_subdivision(to_string(user_uuid))));
            json->addMember(arcirk::content_value("department", get_user_department(to_string(user_uuid))));
        } else
            json->addMember(arcirk::content_value("name", "не зарегистрирован"));

        websocket_session* session = nullptr;

        auto sessions = get_sessions(user_uuid);

        if (sessions.size() > 0){
            json->addMember(arcirk::content_value("active", true));
        } else
            json->addMember(arcirk::content_value("active", false));

        custom_result = json->to_string();
        delete json;
    } else{
        err = "Ошибка получения данных пользователя!";
        return false;
    }

    return true;
}

bool shared_state::get_group_list(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                  std::string &err, std::string &custom_result){

    std::string query = "SELECT SecondField, FirstField, Ref, Parent FROM Channels;";
    err = "";
    sqlite3Db->execute(query, "Channels", custom_result, err);

    if (!err.empty()){
        return false;
    }

    return true;
}

bool shared_state::add_group(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
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

    std::string group_name = params->get_member("name").get_string();
    std::string group_presentation = params->get_member("presentation").get_string();
    std::string group_parent = params->get_member("parent").get_string();
    boost::uuids::uuid _uuid = boost::uuids::random_generator()();

    std::string _group_parent;
    if (group_parent.empty()){
        _group_parent = arcirk::nil_string_uuid();
    } else
        _group_parent = group_parent;

    std::vector<arcirk::content_value> fields;

    fields.emplace_back(arcirk::content_value("FirstField", group_name));
    fields.emplace_back(arcirk::content_value("SecondField", group_presentation));
    fields.emplace_back(arcirk::content_value("Ref", arcirk::uuid_to_string(_uuid)));
    fields.emplace_back(arcirk::content_value("Parent", _group_parent));

    err = "";
    sqlite3Db->insert(arc_sqlite::tables::eChannels, fields, err);

    if (!err.empty())
        return false;
    else{
        auto * obj = new arcirk::bJson();
        obj->set_object();
        obj->addObject(fields);
        custom_result = obj->to_string();
        delete obj;
        return true;
    }

}
bool shared_state::edit_group(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
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

    std::string group_name = params->get_member("name").get_string();
    std::string group_presentation = params->get_member("presentation").get_string();
    std::string group_parent = params->get_member("parent").get_string();
    std::string _uuid = params->get_member("ref").get_string();

    if (_uuid.empty()){
        err = "не указан идентификатор группы!";
        return false;
    }

    std::vector<arcirk::content_value> _sets;

    _sets.emplace_back(arcirk::content_value("FirstField", group_name));
    _sets.emplace_back(arcirk::content_value("SecondField", group_presentation));
    _sets.emplace_back(arcirk::content_value("Parent", group_parent));

    std::vector<arcirk::content_value> _where;
    _where.emplace_back(arcirk::content_value("Ref", _uuid));

    err = "";
    sqlite3Db->update(arc_sqlite::tables::eChannels, _sets, _where, err);

    if (!err.empty())
        return false;
    else{
        _sets.emplace_back(arcirk::content_value("Ref", _uuid));
        auto * obj = new arcirk::bJson();
        obj->set_object();
        obj->addObject(_sets);
        custom_result = obj->to_string();
        delete obj;
        return true;
    }
}

bool shared_state::remove_group(boost::uuids::uuid &uuid, arcirk::bJson* params, ws_message *msg,
                                std::string &err, std::string &custom_result)  {
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

    std::string _uuid = params->get_member("ref").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор группы!";
        return false;
    }

    //удаляем группу вместе с подчиненными элементами
    //пользователей перемещаем в root группу
    remove_group_hierarchy(_uuid, err);
    if(!err.empty())
        return false;
    custom_result = params->to_string();

    return true;
}

void shared_state::remove_group_hierarchy(const std::string &current_uuid, std::string &err) {

    std::set<int> m_idList;
    std::string query = "SELECT _id FROM Channels WHERE Ref = '" + current_uuid + "';";
    std::vector<std::map<std::string, arcirk::bVariant>> table;

    //Получаем _id верхнего узла иерархии по uuid и добавляем его в общий список set
    err = "";
    int result = sqlite3Db->execute(query, "Channels", table, err);

    if (!err.empty()){
        return;
    }
    if (result > 0){
        std::map<std::string, arcirk::bVariant> row = table[0];
        if(row["_id"].is_int())
            m_idList.insert(row["_id"].get_int());
    }

    if (m_idList.empty()){
        err = "Группа в списке не найдена!";
        return;
    }

    //собираем идентификаторы всей иерархии рекурсивно
    get_group_hierarchy_keys(current_uuid, m_idList, err);

    //подготавливаем список ключей для перемещения пользователей в группу root
    query = "SELECT Users._id FROM Users INNER JOIN Channels ON Users.channel = Channels.Ref WHERE Channels._id IN (";

    for (auto itr = m_idList.begin(); itr != m_idList.end() ; ++itr) {
        query.append(std::to_string(*itr));
        if (itr !=  --m_idList.end())
            query.append(",");
    }
    query.append(");");
    std::vector<std::map<std::string, arcirk::bVariant>> tableUsers;
    err = "";
    sqlite3Db->execute(query, "Users", tableUsers, err);
    if (!err.empty()){
        return;
    }

    //Перемещаем
    query = "UPDATE Users SET channel = '" + arcirk::nil_string_uuid() + "' WHERE _id in (";
    for (auto itr = tableUsers.begin(); itr != tableUsers.end() ; ++itr) {
        std::map<std::string, arcirk::bVariant> row = *itr;
        if(row["_id"].is_int())
            query.append(std::to_string(row["_id"].get_int()));
        if (itr !=  --tableUsers.end())
            query.append(",");
    }
    query.append(");");
    err = "";
    sqlite3Db->exec(query, err);

    std::cout << query << std::endl;

    if (!err.empty()){
        return;
    }

    //удаляем группы
    query = "DELETE FROM Channels WHERE _id in (";
    for (auto itr = m_idList.begin(); itr != m_idList.end() ; ++itr) {
        query.append(std::to_string(*itr));
        if (itr !=  --m_idList.end())
            query.append(",");
    }
    query.append(");");
    err = "";
    sqlite3Db->exec(query, err);

}

void shared_state::get_group_hierarchy_keys(const std::string &current_uuid, std::set<int> &vec_uuid, std::string &err) {

    std::string query = "select _id, Ref from Channels where Parent = '" + current_uuid + "';";

    std::vector<std::map<std::string, arcirk::bVariant>> table;
    err = "";
    int result = sqlite3Db->execute(query, "Channels", table, err);

    if (result > 0){
        std::map<std::string, arcirk::bVariant> row = table[0];
        if(row["_id"].is_int())
            vec_uuid.insert(row["_id"].get_int());

        std::string parent = row["Ref"].get_string();
        get_group_hierarchy_keys(parent, vec_uuid, err);
    }

}

bool shared_state::set_parent(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
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
    std::string _uuid = params->get_member("user").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!";
        return false;
    }

    std::string _parent = params->get_member("parent").get_string();
    if (_uuid.empty()){
        _parent = arcirk::nil_string_uuid();
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

bool shared_state::remove_user(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
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

    std::string _uuid = params->get_member("ref").get_string();
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

bool shared_state::kill_session(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
                                std::string &err, std::string &custom_result){

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

    std::string _uuid = params->get_member("ref").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!!";
        return false;
    }

    boost::uuids::uuid kill_uuid = arcirk::string_to_uuid(_uuid, false);
    auto  kill_sess = get_session(kill_uuid);
    if (kill_sess)
        kill_sess->close();
    else
        return false;

    return true;

}

//bool shared_state::set_uuid(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
//                            std::string &err, std::string &custom_result) {
//
//    auto  current_sess = get_session(uuid);
//
//    try {
//        current_sess->throw_authorized();
//    }catch (boost::exception const &e) {
//        err = boost::diagnostic_information(e);
//        std::cerr << err << std::endl;
//        return false;
//
//    }
//
//    if (current_sess->get_role() != "admin"){
//        err = "не достаточно прав доступа для команды!";
//        return false;
//    }
//
//    std::string _uuid = params->get_member("uuid_set").get_string();
//    if (_uuid.empty()){
//        err = "не указан идентификатор сессии!!";
//        return false;
//    }
//
//    boost::uuids::uuid uuid_set = arcirk::string_to_uuid(_uuid, false);
//    auto  session_set = get_session(uuid_set);
//    if (!session_set) //такой сессии не существует
//        return false;
//
//    std::string _new_uuid = params->get_member("new_uuid").get_string();
//    if (_new_uuid.empty())
//        return false;
//
//    boost::uuids::uuid new_uuid = arcirk::string_to_uuid(_new_uuid, false);
//
//    auto  session_valid = get_session(uuid_set);
//    if (session_valid) //сессия с таким идентификатором уже  существует
//        return false;
//
//    //обновляем uuid сессии
//    sessions_.erase(uuid_set);
//    session_set->set_uuid(new_uuid);
//    sessions_.insert(std::pair<boost::uuids::uuid, websocket_session*>(session_set->get_uuid(), session_set));
//
//    return true;
//}

bool shared_state::set_app_name(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
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

    std::string _uuid = params->get_member("uuid_set").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор сессии!!";
        return false;
    }

    boost::uuids::uuid uuid_set = arcirk::string_to_uuid(_uuid, false);

    std::string new_app_name = params->get_member("new_app_name").get_string();
    if (new_app_name.empty())
        return false;

    if (uuid != uuid_set){
        auto  session_set = get_session(uuid_set);
        if (session_set)
            session_set->set_app_name(new_app_name);
        else
            return false;
    } else
        current_sess->set_app_name(new_app_name);

    return true;

}

bool shared_state::exec_query(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
                              std::string &err, std::string &custom_result) {

    std::string query = params->get_member("query").get_string();
    if (query.empty())
        return false;

    err = "";
    std::string szResult;
    int result = sqlite3Db->execute(query, "", szResult, err);
    if (err.empty())
        custom_result = szResult;
    else
        return false;

    return true;

}

bool shared_state::get_users_catalog(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg,
                                     std::string &err, std::string &custom_result) {
//    std::string query = "SELECT FirstField, SecondField, Ref, Parent, 1 AS IsGroup FROM Channels "
//                        "UNION "
//                        "SELECT FirstField, SecondField, Ref, channel AS Parent, 0 AS IsGroup FROM Users;";

    std::string query = "select * from UsersCatalog;";
    err = "";
    std::string szResult;
    int result = sqlite3Db->execute(query, "UsersCatalog", szResult, err, true);
    if (err.empty())
        custom_result = szResult;
    else
        return false;

    return true;
}

bool shared_state::get_user_cache(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg, std::string &err,
                                   std::string &custom_result) {

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

    std::string _uuid = params->get_member("ref").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!!";
        return false;
    }

    std::string query = arcirk::str_sample("SELECT Users.cache WHERE Users.Ref = '%1'", _uuid);

    std::vector<std::map<std::string, arcirk::bVariant>> table;
    err = "";

    int result = sqlite3Db->execute(query, "Users", table, err);

    if (!err.empty()){
        custom_result = "{}";
    }
    if (result > 0){
        custom_result = table[0]["cache"].get_string();
    }

    custom_result = "{}";

    return true;
}

bool shared_state::save_user_cache(boost::uuids::uuid &uuid, arcirk::bJson *params, ws_message *msg, std::string &err,
                                  std::string &custom_result) {

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

    std::string _uuid = params->get_member("ref").get_string();
    if (_uuid.empty()){
        err = "не указан идентификатор пользователя!!";
        return false;
    }

    std::string cache = params->get_member("cache").get_string();

    std::string query = arcirk::str_sample("UPDATE Users SET cache = '%1' WHERE Ref = '%2'", cache, _uuid);

    err = "";
    sqlite3Db->exec(query, err);
    if(!err.empty())
        return false;

    custom_result = params->to_string();

    return true;
}

std::string shared_state::get_user_subdivision(const std::string &user_ref) {

    std::string sample = "SELECT UsersCatalog.channel, UsersCatalog.SecondField, UsersCatalog.Ref FROM UsersCatalog WHERE %1 = '%2' %3;";
    std::string query = arcirk::str_sample(sample, "UsersCatalog.Ref", user_ref, "");
    std::string err = "";
    std::vector<std::map<std::string, arcirk::bVariant>> table;
    err = "";

    int result = sqlite3Db->execute(query, "UsersCatalog", table, err);

    if (!err.empty()){
        return "{}";
    }
    if (result > 0){
        std::string parent = table[0]["channel"].get_string();
        while (result > 0){
            table.clear();
            if(!parent.empty()){
                break;
            }
            query = arcirk::str_sample(sample, "UsersCatalog.channel", parent, "AND UsersCatalog.IsGroup = 1");
            result = sqlite3Db->execute(query, "UsersCatalog", table, err);
        }

        if (table.size() > 0){
            return  arcirk::str_sample("{\"uuid\": \"%1\", \"name\"; \"%2\"",
                                       table[0]["Ref"].get_string(),
                                       table[0]["SecondField"].get_string());
        }
    }

    return "{}";
}

std::string shared_state::get_user_department(const std::string &user_ref) {

    std::string sample = "SELECT UsersCatalog.channel, UsersCatalog.SecondField, UsersCatalog.Ref FROM UsersCatalog WHERE %1 = '%2' %3;";
    std::string query = arcirk::str_sample(sample, "UsersCatalog.Ref", user_ref, "");
    std::string err = "";
    std::vector<std::map<std::string, arcirk::bVariant>> table;
    err = "";

    int result = sqlite3Db->execute(query, "UsersCatalog", table, err);

    if (!err.empty()){
        return "{}";
    }
    if (result > 0){
        std::string parent = table[0]["channel"].get_string();
        query = arcirk::str_sample(sample, "UsersCatalog.channel", parent, "AND UsersCatalog.IsGroup = 1");
        result = sqlite3Db->execute(query, "UsersCatalog", table, err);

        if (result > 0){
            return  arcirk::str_sample("{\"uuid\": \"%1\", \"name\"; \"%2\"",
                                       table[0]["Ref"].get_string(),
                                       table[0]["SecondField"].get_string());
        }
    }
    return "{}";
}
