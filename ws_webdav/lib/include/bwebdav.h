//
// Created by arcady on 27.01.2022.
//

#ifndef WS_SOLUTION_BWEBDAV_H
#define WS_SOLUTION_BWEBDAV_H

#include <iostream>

#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

// Report a failure

namespace arcirk{
    //------------------------------------------------------------------------------
    namespace beast = boost::beast;     // from <boost/beast.hpp>
    namespace http = beast::http;       // from <boost/beast/http.hpp>
    namespace net = boost::asio;        // from <boost/asio.hpp>
    using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
    namespace ssl = net::ssl;

// Performs an HTTP GET and prints the response
    class session : public std::enable_shared_from_this<session>
    {
        tcp::resolver resolver_;
        beast::ssl_stream<beast::tcp_stream> stream_;
        beast::flat_buffer buffer_; // (Must persist between reads)
        http::request<http::empty_body> req_;
        http::response<http::string_body> res_;

    public:
        explicit
        session(
                net::any_io_executor ex,
                ssl::context& ctx)
                : resolver_(ex)
                , stream_(ex, ctx)
        {
        }

        // Start the asynchronous operation
        void
        run(
                char const* host,
                char const* port,
                char const* target,
                int version)
        {
            // Set SNI Hostname (many hosts need this to handshake successfully)
            if(! SSL_set_tlsext_host_name(stream_.native_handle(), host))
            {
                beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
                std::cerr << ec.message() << "\n";
                return;
            }

            // Set up an HTTP GET request message
            req_.version(version);
            req_.method(http::verb::get);
            req_.target(target);
            req_.set(http::field::host, host);
            req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // Look up the domain name
            resolver_.async_resolve(
                    host,
                    port,
                    beast::bind_front_handler(
                            &session::on_resolve,
                            shared_from_this()));
        }

        void
        on_resolve(
                beast::error_code ec,
                tcp::resolver::results_type results)
        {
            if(ec)
                return fail(ec, "resolve");

            // Set a timeout on the operation
            beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

            // Make the connection on the IP address we get from a lookup
            beast::get_lowest_layer(stream_).async_connect(
                    results,
                    beast::bind_front_handler(
                            &session::on_connect,
                            shared_from_this()));
        }

        void
        on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
        {
            if(ec)
                return fail(ec, "connect");

            // Perform the SSL handshake
            stream_.async_handshake(
                    ssl::stream_base::client,
                    beast::bind_front_handler(
                            &session::on_handshake,
                            shared_from_this()));
        }

        void
        on_handshake(beast::error_code ec)
        {
            if(ec)
                return fail(ec, "handshake");

            // Set a timeout on the operation
            beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

            // Send the HTTP request to the remote host
            http::async_write(stream_, req_,
                              beast::bind_front_handler(
                                      &session::on_write,
                                      shared_from_this()));
        }

        void
        on_write(
                beast::error_code ec,
                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return fail(ec, "write");

            // Receive the HTTP response
            http::async_read(stream_, buffer_, res_,
                             beast::bind_front_handler(
                                     &session::on_read,
                                     shared_from_this()));
        }

        void
        on_read(
                beast::error_code ec,
                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return fail(ec, "read");

            // Write the message to standard out
            std::cout << res_ << std::endl;

            // Set a timeout on the operation
            beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

            // Gracefully close the stream
            stream_.async_shutdown(
                    beast::bind_front_handler(
                            &session::on_shutdown,
                            shared_from_this()));
        }

        void
        on_shutdown(beast::error_code ec)
        {
            if(ec == net::error::eof)
            {
                // Rationale:
                // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
                ec = {};
            }
            if(ec)
                return fail(ec, "shutdown");

            // If we get here then the connection is closed gracefully
        }

    private:
        void
        fail(boost::beast::error_code ec, char const* what)
        {
            std::cerr << what << ": " << ec.message() << "\n";
        }
    };

    class bWebDav final{
    public:
        explicit
        bWebDav(net::io_context &io_context,
                std::string host,
                int port = 80,
                std::string  user = "",
                std::string  pwd = "",
                std::string root_dir = "",
                bool ssl = true);

    bool check_connection();

    private:
        std::string m_host;
        int m_port;
        bool m_ssl;
        std::string m_user;
        std::string m_pass;
        std::string m_root_dir;

        boost::asio::io_context &ioc;

    };

}



#endif //WS_SOLUTION_BWEBDAV_H
