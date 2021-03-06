//
// Created by arcady on 03.02.2022.
//

#include "../include/synch_session.h"
#include <arcirk.h>
#include <server_response.h>

sync_session::sync_session(net::io_context &ioc)
: resolver_(net::make_strand(ioc))
, ws_(net::make_strand(ioc))
{

}

bool sync_session::run(const char *host, const char *port) {

    auto const results = resolver_.resolve(host, port);
    auto ep = net::connect(ws_.next_layer(), results);

    host_ = host;
    host_ += ':' + std::to_string(ep.port());

    ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req) {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");
            }));
    ws_.handshake(host, "/");

}

void sync_session::write(const std::string &msg) {

    try {
        if(!ws_.is_open()){
            std::cerr << "sync_session::set_param: socket not open" << std::endl;
            return;
        }

        ws_.write(net::buffer(std::string(msg)));

    } catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void sync_session::read() {
    try {
        if(!ws_.is_open()){
            std::cerr << "sync_session::set_param: socket not open" << std::endl;
            return;
        }

        buffer_.consume(buffer_.size());
        ws_.read(buffer_);

    } catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string sync_session::get_buffer() const {
    if (buffer_.size() == 0)
        return {};

    return beast::buffers_to_string(buffer_.data());
}

void sync_session::clear_buffer() {
    buffer_.consume(buffer_.size());
}

void sync_session::close() {
    try {
        if(ws_.is_open())
            ws_.close(websocket::close_code::normal);
    } catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}

bool sync_session::set_param(const std::string &usr, const std::string &pwd) {
    try {
        if(!ws_.is_open()){
            std::cerr << "sync_session::set_param: socket not open" << std::endl;
            return false;
        }

        auto param = arcirk::bJson();
        param.set_object();
        param.addMember(arcirk::content_value("uuid", arcirk::random_uuid()));
        param.addMember(arcirk::content_value("name", usr));
        std::string hash = arcirk::get_hash(usr, pwd);
        param.addMember(arcirk::content_value("hash", hash));
        param.addMember(arcirk::content_value("app_name", "client_sync"));
        param.addMember(arcirk::content_value("user_uuid", arcirk::random_uuid()));
        std::string client_param = param.to_string();

        auto _msg = arcirk::ws_message();
        _msg.message().uuid = arcirk::string_to_uuid(arcirk::random_uuid());
        _msg.message().message = client_param;
        _msg.message().name = usr;
        _msg.message().app_name = "client_sync";
        _msg.message().command = "set_client_param";
        _msg.message().uuid_form = arcirk::nil_uuid();

        std::string msg = "cmd " + _msg.get_json(true);

        ws_.write(net::buffer(std::string(msg)));

        buffer_.consume(buffer_.size());
        ws_.read(buffer_);

        if (buffer_.size() > 0){
            //std::string json = arcirk::base64_decode(beast::buffers_to_string(buffer.data()));
            arcirk::T_vec v = arcirk::split(beast::buffers_to_string(buffer_.data()), "\n");

            if (v[0].substr(0, 6) == "result") {

                std::string base64 = v[0].substr(7, v[0].length() - 6);
                std::string result = arcirk::base64_decode(base64);
                auto resp = new ServerResponse(result);
                if(resp->isParse)
                {
                    if(resp->result == "error"){
                        std::cerr << resp->command << ": "  << resp->message << std::endl;
                        return false;
                    }
                    else
                    {
                        std::cout << resp->command << ": " << resp->message << std::endl;
                        return true;
                    }
                }else{
                    std::cerr << "???????????? ???????????? ??????????????????!";
                    return false;
                }

            }else{
                std::cout << arcirk::base64_decode(beast::buffers_to_string(buffer_.data())) << std::endl;
                return false;
            }

            //
        }
    } catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return false;
}

bool sync_session::is_open() {
    return ws_.is_open();
}

sync_session::~sync_session() = default;