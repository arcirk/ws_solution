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

void subscriber::set_uuid(boost::uuids::uuid &uuid) {
    uuid_ = boost::uuids::uuid(uuid);
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
