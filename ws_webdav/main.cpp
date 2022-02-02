//
// Created by arcady on 27.01.2022.
//

#include <iostream>
#include <bwebdav.h>
#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio/connect.hpp>
//#include <boost/asio/ip/tcp.hpp>
////#include <boost/beast/ssl.hpp>
////#include <boost/asio/ssl/error.hpp>
////#include <boost/asio/ssl/stream.hpp>
//#include <cstdlib>
//#include <iostream>
//#include <string>
//
//namespace beast = boost::beast;     // from <boost/beast.hpp>
//namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
//using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
//namespace ssl = net::ssl;

bool check_connection() {
//    try
//    {
//        net::io_context ioc;
//
////        ssl::context ctx(ssl::context::tlsv12_client);
////        load_root_certificates(ctx);
////        ctx.set_verify_mode(ssl::verify_peer);
//
//        std::string m_host = "arcirk.ru";
//
//        tcp::resolver resolver(ioc);
//        beast::tcp_stream stream(ioc);
//        auto const results = resolver.resolve(m_host, std::to_string(443));
//        stream.connect(results);
//
//        //auto const ressults = resolver.resolve(m_host, std::to_string(80));
//        // Set up an HTTP GET request message
//        http::request<http::string_body> req{http::verb::get, "remote.php/dav/files/webDavUser", 11};
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
//        beast::error_code ec;
//        stream.socket().shutdown(tcp::socket::shutdown_both, ec);
//        if(ec && ec != beast::errc::not_connected)
//            throw beast::system_error{ec};
//    }
//    catch(std::exception const& e)
//    {
//        std::cerr << "Error: " << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv){

    net::io_context ioc;
    auto dav = arcirk::bWebDav(ioc, "arcirk.ru", 443, "webDavUser", "@LbyFvj1", "remote.php/dav/files");

    bool result = dav.check_connection();

    return 1;
}