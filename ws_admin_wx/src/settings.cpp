//
// Created by arcady on 09.08.2021.
//
#include <iostream>
#include "../include/settings.h"

void settings::get_settings() {

    bool error = false;

    pt.clear();

    try {
        const std::string filename = "../conf/conf.json";

        boost::property_tree::json_parser::read_json(filename, pt);
    }catch (boost::exception &e){
        std::cerr << "Ошибка открытия файла\n";
        error = true;
    }

    if (error){

        pt.put("root", "root");
        pt.put_child( "root.servers", ptree() );
    }

}

void settings::save_settings() {

    if (pt.empty())
        return;

}

void settings::add_server(srv_info *srvInfo) {

    boost::property_tree::ptree::const_assoc_iterator i_pts = pt.find("root");
    if(pt.not_found() != i_pts) {

        boost::property_tree::ptree servers = pt.get_child( "root.servers" );

        boost::property_tree::ptree elem;
        elem.put("host", srvInfo->host);
        elem.put("port", srvInfo->port);
        elem.put("user", srvInfo->user);
        elem.put("pwd", srvInfo->pwd);
        //pt.put_child("root",)
        servers.push_back(std::make_pair("", elem));

    }
    //const boost::property_tree::ptree & pts = (*i_pts).second;
}