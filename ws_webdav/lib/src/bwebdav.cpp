//
// Created by arcady on 27.01.2022.
//

#include "../include/bwebdav.h"


#include <arcirk.h>

#include <utility>


using namespace arcirk;

bWebDav::bWebDav(net::io_context &io_context, std::string host, int port, std::string user, std::string pwd,
    std::string root_dir, bool ssl)
    : m_host(std::move(host)),
    m_port(port),
    m_user(std::move(user)),
    m_pass(std::move(pwd)),
    m_root_dir(std::move(root_dir)),
    m_ssl(ssl),
    ioc(io_context){

}

bool bWebDav::check_connection() {
    //try
    //{
        //net::io_context ioc;
        std::string target = "/" + m_root_dir + "/" + m_user;
        int version = 11;
        ssl::context ctx(ssl::context::tlsv12_client);
        load_root_certificates(ctx);
        ctx.set_verify_mode(ssl::verify_peer);
        std::make_shared<session>(
                net::make_strand(ioc),
                ctx
        )->run(m_host.c_str(), std::to_string(m_port).c_str(), target.c_str(), version);
        ioc.run();
        return EXIT_SUCCESS;

//        //ssl::context ctx(boost::asio::ssl::context::sslv23);
//
//        //ctx.set_verify_mode(ssl::verify_peer);
//
//        tcp::resolver resolver(ioc);
//        //beast::tcp_stream stream(ioc);
//        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
//        // Set SNI Hostname (many hosts need this to handshake successfully)
//        if(! SSL_set_tlsext_host_name(stream.native_handle(), m_host.c_str()))
//        {
//            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
//            throw beast::system_error{ec};
//        }
//        auto const results = resolver.resolve(m_host, std::to_string(m_port));
//        //stream.connect(results);
//        beast::get_lowest_layer(stream).connect(results);
//
//        stream.handshake(ssl::stream_base::client);
//
//        //auto const ressults = resolver.resolve(m_host, std::to_string(80));
//        // Set up an HTTP GET request message
//        http::request<http::string_body> req{http::verb::propfind, "/" + m_root_dir + "/" + m_user, 11};
//        req.set(http::field::host, m_host);
//        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//
//        std::string concatenated = m_user + ":" + m_pass;
//        std::string data = base64_encode(concatenated);
//        std::string headerData = "Basic " + data;
//        req.set(http::field::authorization, headerData);
//        req.set(http::field::depth, "1");
//        req.set(http::field::content_type, "application/x-www-form-urlencoded; charset=UTF-8");
//
//        http::write(stream, req);
//        beast::flat_buffer buffer;
//        http::response<http::dynamic_body> res;
//        http::read(stream, buffer, res);
//        std::cout << res << std::endl;
//
//        beast::error_code ec;

//        beast::get_lowest_layer(stream).cancel();
//        //auto _ssl_socket = beast::get_lowest_layer(stream);
//       // stream.async_shutdown([](...) { beast::get_lowest_layer(stream).close(); };
//
////        beast::get_lowest_layer(stream).close();
////        stream.socket().shutdown(tcp::socket::shutdown_both, ec);
////        if(ec && ec != beast::errc::not_connected)
////            throw beast::system_error{ec};
//        //stream.shutdown(ec);
//        if(ec == net::error::eof)
//        {
//            // Rationale:
//            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
//            ec = {};
//        }
//        if(ec)
//            throw beast::system_error{ec};
//        beast::get_lowest_layer(stream).cancel();
//        stream.shutdown(ec);
//        if(ec == boost::asio::error::eof)
//        {
//            //This is fine. I am okay with the events that are unfolding currently.
//            ec.assign(0, ec.category());
//        }
//        if(ec)
//        {
//            std::cout << "Got error code: " << ec << " on socket cleanup in SSL shutdown" << std::endl;
//        }
//
//        if(ec)
//            throw beast::system_error{ec};
//
//        //stream.async_shutdown([](...) { beast::get_lowest_layer(stream).close(); }
//
////        if(ec)
////        {
////            std::cout << "Got error code: " << ec << " on socket cleanup in TCP socket shutdown." << std::endl;
////        }
//    }
//    catch(std::exception const& e)
//    {
//        std::cerr << "Error: " << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }
//    return EXIT_SUCCESS;
}