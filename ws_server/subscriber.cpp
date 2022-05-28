//
// Created by arcady on 14.07.2021.
//

#include "./include/subscriber.h"



boost::uuids::uuid & subscriber::get_uuid() {
    return uuid_;
}

boost::uuids::uuid & subscriber::get_user_uuid() {
    return user_uuid_;
}

std::string subscriber::get_name() const {
    return sub_name_;
}

void subscriber::set_name(std::string &name) {
    sub_name_ = name;
}

std::string subscriber::get_user_name() const {
    return _user_name;
}

void subscriber::set_user_name(std::string &name) {
    _user_name = name;
}

void subscriber::set_user_uuid(boost::uuids::uuid &uuid) {
    user_uuid_ = boost::uuids::uuid(uuid);
}

void subscriber::set_app_name(const std::string &app_name) {
    app_name_ = app_name;
}

std::string subscriber::get_app_name() const {
    return app_name_;
}

const std::string &subscriber::get_role() {
    return role_;
}

void subscriber::set_role(const std::string &role) {
    role_ = role;
}

void subscriber::set_content_type(const std::string &msg_format) {
    contentType = msg_format;
}

std::string subscriber::get_content_type() const{
    return contentType;
}

void subscriber::set_message_struct_type(const std::string &struct_type) {
    structType = struct_type;
}

std::string subscriber::get_message_struct_type() const {
    return structType;
}
