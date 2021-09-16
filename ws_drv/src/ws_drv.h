/*
 *  Modern Native AddIn
 *  Copyright (C) 2018  Infactum
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SAMPLEADDIN_H
#define SAMPLEADDIN_H

#include "Component.h"

#ifdef _WINDOWS
    #include "../../ws_client/include/net.h"
    #include "../../ws_client/include/ws_client.h"
#else
    #include "./ws_client.h"
#endif // _WINDOWS


class ws_drv final : public Component {
public:
    const char *Version = u8"1.0.0";
    std::shared_ptr<variant_t> port;
    std::shared_ptr<variant_t> server;
    std::shared_ptr<variant_t> user_uuid;
    std::shared_ptr<variant_t> client_uuid;
    std::shared_ptr<variant_t> log_path;

    ws_drv();

    void ext_message(const std::string& msg);

private:
    std::string extensionName() override;

    void message(const variant_t &msg);

    int open();

    int open_as(const variant_t &param);

    //int open_as(const std::string& param);

    void close();

    void send(const variant_t& msg, const variant_t& sub_user_uuid, const variant_t& uuid_form);

    void start();

//    void start(const std::string &uuid, const std::string &name, const std::string &pwd,
//               const std::string &s_user_uuid);

    void start_();

    bool started();

    variant_t get_client_info();

    std::string get_current_name();

    void join_channel(const variant_t &uuid, const variant_t &uuid_form);

    void close_channel(const variant_t &uuid, const variant_t &uuid_form);

    void set_log(const std::string &msg, const std::string &filename);

    void to_channel(const variant_t& msg, const variant_t& uuid_sub, const variant_t& uuid_form);

    //Команда серверу
    void send_command(const variant_t& cmd, const variant_t& uuid_form, const variant_t& param);
    void send_command_(const std::string &cmd, const std::string &uuid_form, const std::string &param);

    std::string get_hash(const variant_t &usr, const variant_t &pwd);
    variant_t currentDate();
    variant_t current_date_in_seconds();
    variant_t get_tz_offset();
    void get_messages(const variant_t &uuid_sub, const variant_t &start_date, const variant_t &end_date, const variant_t &limit, const variant_t &uuid_form);
    void get_user_info(const variant_t &user_uuid, const variant_t &uuid_form);
    void get_group_list(const variant_t &uuid_form);
    void add_group(const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form);
    void edit_group(const variant_t &uuid_group, const variant_t &name, const variant_t &presentation, const variant_t &uuid_parent, const variant_t &uuid_form);
    void remove_group(const variant_t &uuid_group, const variant_t &uuid_form);
    void get_users(const variant_t &uuid_group, const variant_t &uuid_form);
    void set_parent(const variant_t &user_uuid, const variant_t &uuid_group, const variant_t &uuid_form);
    void remove_user(const variant_t &user_uuid, const variant_t &uuid_form);

    ws_client* client;
    variant_t _client_param;

};

#endif //SAMPLEADDIN_H
