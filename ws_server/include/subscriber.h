//
// Created by arcady on 14.07.2021.
//

#ifndef WS_SOLUTION_SUBSCRIBER_H
#define WS_SOLUTION_SUBSCRIBER_H

#include <iostream>
#include <memory>
#include <set>
#include <map>
#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

class subscriber {

public:
    boost::uuids::uuid	uuid_;
    std::string	sub_name_	= "не определен";
    bool authorized = false;
    boost::uuids::uuid	user_uuid_;
    std::string app_name_ = "Console";
    std::string role_ = "user";
    std::string contentType = "TEXT"; //формат сообщения
    std::string structType = "TEXT"; //структура сообщения "DB" = строка базы данных для моделей QT
    //explicit
    //subscriber(std::string  name, boost::uuids::uuid &&uuid);

    virtual ~subscriber() = default;
    virtual void deliver(const boost::shared_ptr<const std::string> &msg) = 0;
    virtual boost::uuids::uuid & get_uuid();
    virtual boost::uuids::uuid & get_user_uuid();
    virtual const std::string & get_role();

    [[nodiscard]] std::string get_name() const;
    void set_name(std::string &name);
    //void set_uuid(boost::uuids::uuid &uuid);
    void set_user_uuid(boost::uuids::uuid &uuid);
    void set_app_name(const std::string &app_name);
    [[nodiscard]] std::string get_app_name() const;
    void set_role(const std::string &role);

    void set_content_type(const std::string& msg_format = "TEXT");
    std::string get_content_type() const;
    void set_message_struct_type(const std::string& struct_type = "TEXT");
    std::string  get_message_struct_type() const;
private:


};

typedef boost::shared_ptr<subscriber> subscriber_ptr;

#endif //WS_SOLUTION_SUBSCRIBER_H
