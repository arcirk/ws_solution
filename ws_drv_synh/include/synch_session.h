//
// Created by arcady on 03.02.2022.
//

#ifndef WS_SOLUTION_SYNC_SESSION_H
#define WS_SOLUTION_SYNCH_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

//#include <server_response.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class sync_session : public boost::enable_shared_from_this<sync_session>
{
    tcp::resolver resolver_;
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
public:
    explicit
    sync_session(net::io_context& ioc);

    ~sync_session();

    bool
    run(char const* host, char const* port);

    void
    write(const std::string& msg);

    void
    read();

    std::string get_buffer() const;
    void clear_buffer();

    void
    close();

    bool set_param(const std::string& usr, const std::string& pwd);

    bool is_open();

};


#endif //WS_SOLUTION_SYNC_SESSION_H
