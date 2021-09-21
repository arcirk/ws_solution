//
// Created by arcady on 13.07.2021.
//
#include <iostream>
#include "../include/ws_session.h"
#include "../include/ws_client.h"

session::
session(net::io_context& ioc)
: resolver_(net::make_strand(ioc))
, ws_(net::make_strand(ioc))
, deadline_(ioc)
, heartbeat_timer_(ioc)
{
    stopped_ = true;
}

session::~session() = default;

// Start the asynchronous operation
void
session::run(
        char const* host,
        char const* port, ws_client * client)
{

    client_ = client;

    // Save these for later
    host_ = host;
    //text_ = text;

    deliver("\n");
    //deliver(client_->get_param());

    std::cout << "connect to server..." << std::endl;

    deadline_.async_wait(std::bind(&session::check_deadline, this));

    std::cout << "start resolve " << host << ' ' << port << std::endl;

    // Look up the domain name
    resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                    &session::on_resolve,
                    shared_from_this()));
}
void
session::on_resolve(
        beast::error_code ec,
        tcp::resolver::results_type results)
{
    if(ec)
        return fail(ec, "resolve");

    // Set the timeout for the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                    &session::on_connect,
                    shared_from_this()));
}
void
session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if(ec){
        stopped_ = true;
        return fail(ec, "connect");
    }
    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-async");
            }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host_ += ':' + std::to_string(ep.port());

    // Perform the websocket handshake
    ws_.async_handshake(host_, "/",
                        beast::bind_front_handler(
                                &session::on_handshake,
                                shared_from_this()));
                }

                void
                session::on_handshake(beast::error_code ec)
                {
    if(ec)
        return fail(ec, "handshake");

    std::cout << "successful connection!" << std::endl;

    stopped_ = false;

    client_->on_connect(this);

    start_write();

    start_read();

}

void
session::start_read()
{
    //Установка крайнего срока для операции чтения.
    deadline_.expires_after(std::chrono::seconds(30));

    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));
}

void
session::on_read(
        beast::error_code ec,
        std::size_t bytes_transferred){
    if (stopped_)
        return;

    boost::ignore_unused(bytes_transferred);

    if(ec){
        stop();
        return fail(ec, "read");
    }

    client_->on_read(beast::buffers_to_string(buffer_.data()));

    //std::cout << s << buffer_.size() << std::endl;

    //beast::make_printable(buffer_.data());

    buffer_.consume(buffer_.size());
    //buffer_.clear();

    start_read();

}

void
session::start_write()
{
    if (stopped_)
        return;

    ws_.async_write(
            net::buffer(output_queue_.front()),
            beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
}

void
session::on_write(
        beast::error_code ec,
        std::size_t bytes_transferred)
{
    if (stopped_)
        return;

    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    buffer_.consume(buffer_.size());
    //buffer_.clear();

    output_queue_.pop_front();

    if (output_queue_.empty()) {
        output_queue_.emplace_back("\n");
    }

    // Запускаем пинг с периодичностью в 30 секунд

    heartbeat_timer_.expires_after(std::chrono::seconds(30));

    heartbeat_timer_.async_wait(std::bind(&session::start_write, this));
}

void
session::stop()
{
    stopped_ = true;
    deadline_.cancel();
    heartbeat_timer_.cancel();
    client_->on_stop();
    ws_.async_close(websocket::close_code::normal,
        beast::bind_front_handler(
                &session::on_close,
                shared_from_this()));
}

void
session::send(boost::shared_ptr<std::string const> const& ss) {


    if (stopped_)
        return;

    deliver(ss->c_str());

    //сбрасываем таймер для отправки следующего сообщения через секунду
    heartbeat_timer_.expires_after(std::chrono::seconds(1));

}

void
session::on_close(beast::error_code ec)
{
    if(ec)
        return fail(ec, "close");

    stopped_ = true;

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable(buffer_.data()) << std::endl;
}

void
session::fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";

    client_->error(what, ec.message());
}

void
session::deliver(const std::string& msg)
{
    output_queue_.clear();
    if (msg == "\n" || msg == "pong"){
        output_queue_.push_back("\n");
    }else
        output_queue_.push_back(msg + "\n");
}

void
session::check_deadline()
{
    if (stopped_)
        return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadline_.expiry() <= steady_timer::clock_type::now())
    {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        // Close the WebSocket connection
        ws_.async_close(websocket::close_code::normal,
                        beast::bind_front_handler(
                                &session::on_close,
                                shared_from_this()));

        // There is no longer an active deadline. The expiry is set to the
        // maximum time point so that the actor takes no action until a new
        // deadline is set.
        deadline_.expires_at((steady_timer::time_point::max)());
    }

    // Put the actor back to sleep.
    deadline_.async_wait(std::bind(&session::check_deadline, this));


}
