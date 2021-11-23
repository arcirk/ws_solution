//
// Created by arcady on 12.07.2021.
//

#ifndef WS_SOLUTION_NET_H
#define WS_SOLUTION_NET_H

#ifdef _WINDOWS
    #pragma warning (disable: 4001)
    #pragma warning (disable: 4101)
    #pragma warning (disable: 4061)
    #include "../../ws_drv/src/stdafx.h"
#else
	#include <boost/asio.hpp>
#endif // _WINDOWS



namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

#endif //WS_SOLUTION_NET_H
