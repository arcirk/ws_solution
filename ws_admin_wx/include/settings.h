//
// Created by arcady on 09.08.2021.
//

#ifndef WS_SOLUTION_SETTINGS_H
#define WS_SOLUTION_SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

typedef struct srv_info{
public:
    std::string host;
    std::string port;
    std::string user;
    std::string pwd;

    srv_info()= default;;
    ~srv_info() = default;
}srv_info;

class settings{

public:
    settings() = default;
    void get_settings();
    void save_settings();

    void add_server(srv_info* srvInfo);

private:
    ptree pt;

};

#endif //WS_SOLUTION_SETTINGS_H
