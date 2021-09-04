//
// Created by arcady on 05.07.2021.
//

#ifndef UNTITLED1_NET_HPP
#define UNTITLED1_NET_HPP

#ifdef _WINDOWS
#include <SDKDDKVer.h>
#endif

#include <boost/asio.hpp>

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

#endif //UNTITLED1_NET_HPP
