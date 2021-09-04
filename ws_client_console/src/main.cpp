
#include <iostream>
#include "../include/net.h"
#include "../include/global.h"
#include "../include/ws_client.h"

#include <boost/thread/thread.hpp>
//#include <boost/asio.hpp>
#include <string>

#ifdef _WINDOWS
#pragma warning (disable : 4505)
#pragma warning (disable : 4061)
#endif

using std::placeholders::_1;
//using std::placeholders::_2;
typedef std::function<void(std::string)> _callback_message;

ws_client * client;

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) {
    boost::asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return boost::asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

#if defined(_WIN32)
#undef UNICODE
#include <winsock2.h>
// Headers that need to be included after winsock2.h:
#include <iphlpapi.h>
#include <ws2ipdef.h>

#pragma comment(lib, "Iphlpapi.lib")

typedef IP_ADAPTER_UNICAST_ADDRESS_LH Addr;
typedef IP_ADAPTER_ADDRESSES *AddrList;

std::vector<boost::asio::ip::address> get_local_interfaces() {
    // It's a windows machine, we assume it has 512KB free memory
    DWORD outBufLen = 1 << 19;
    AddrList ifaddrs = (AddrList) new char[outBufLen];
    struct ifaddrs *ifaddr;

    std::vector<boost::asio::ip::address> res;

    ULONG err = GetAdaptersAddresses(AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, ifaddrs,
        &outBufLen);

    if (err == NO_ERROR) {
        for (AddrList addr = ifaddrs; addr != 0; addr = addr->Next)
            {
            if (addr->OperStatus != IfOperStatusUp) continue;
            // if (addr->NoMulticast) continue;

            // Find the first IPv4 address
            if (addr->Ipv4Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET) continue;
                    res.push_back(boost::asio::ip::make_address_v4(ntohl(reinterpret_cast<sockaddr_in *>(uaddr->Address.lpSockaddr)->sin_addr.s_addr)));
                }
            }

            if (addr->Ipv6Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET6) continue;
                    res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(uaddr->Address.lpSockaddr)));
                }
            }
        }
    } else {

    }
    delete[]((char *)ifaddrs);
    return res;
}
#elif defined(__APPLE__) || defined(__linux__)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>

std::vector<boost::asio::ip::address> get_local_interfaces() {
    std::vector<boost::asio::ip::address> res;
    ifaddrs *ifs;
    if (getifaddrs(&ifs)) {
        return res;
    }
    for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) {
        // No address? Skip.
        if (addr->ifa_addr == nullptr) continue;

        // Interface isn't active? Skip.
        if (!(addr->ifa_flags & IFF_UP)) continue;

        if(addr->ifa_addr->sa_family == AF_INET) {
            res.push_back(boost::asio::ip::make_address_v4(ntohl(
                    reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
        } else if(addr->ifa_addr->sa_family == AF_INET6) {
            res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
        } else continue;
    }
    freeifaddrs(ifs);
    return res;
}
#else
#error "..."
#endif

void ext_message(const std::string& msg) {

    try {
            std::cout << arc_json::base64_decode(msg) << std::endl;

    }catch (std::exception &e){
        std::cout << msg << std::endl;
    }


    // auto success = this->ExternalEvent("WebSocketAddIn", "message", msg);
}
//------------------------------------------------------------------------------
void start(){

    _callback_message callback = [](auto && PH1) { return ext_message(std::forward<decltype(PH1)>(PH1)); };

    // The io_context is required for all I/O
    net::io_context ioc;

    std::string client_param = "{"
                               " \"uuid\": \"00000000-0000-0000-0000-000000000000\","
                               " \"name\": \"admin\","
                               " \"pwd\": \"admin\","
                               " \"app_name\": \"Console\","
                               " \"user_uuid\": \"00000000-0000-0000-0000-000000000000\""
                               "}";

    client = new ws_client(ioc, client_param);
    //std::cout << client->get_hash("admin", "admin") << std::endl;
    client->decode_message = true;
    client->open("192.168.10.80", "8080", callback);

    std::cout << "exit thread" << std::endl;

}

std::string getAddress()
{
    namespace ip = boost::asio::ip;
    boost::asio::io_service ioService;
    ip::tcp::resolver resolver(ioService);

    return resolver.resolve(ip::host_name(), "")->endpoint().address().to_string();
}

void getLocalIPs() {

    //using boost::asio::ip::tcp;

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(boost::asio::ip::host_name(), "");
    tcp::resolver::iterator iter = resolver.resolve(query);
    tcp::resolver::iterator end; // End marker.
    while (iter != end)
    {
        tcp::endpoint ep = *iter++;
        std::cout << ep << std::endl;

    }
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Russian");

    //getLocalIPs();
    //std::cout << getAddress() << std::endl;

    std::vector<boost::asio::ip::address> ip_vec = get_local_interfaces();

    for (auto iter : ip_vec){
        if (iter.to_string().substr(0,7) == "192.168")
            std::cout << iter.to_string() << std::endl;
    }

    //start();

    boost::thread f(start);
    //boost::thread(boost::bind(&WebSocketAddIn::start, this)).detach();

    std::string line;

    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        // Reconnect the client
        if (line == "!")
        {
            continue;
        }
        else if (line == "get_connections")
        {
            //client->run_command("get_connections");
            continue;
        }
        else if (line == "connect")
        {
            if (!client->started())
                boost::thread f(start);
            continue;
        }
        else if (line == "stop")
        {
            client->close();
            continue;
        }
        else if (line == "exit")
        {
            if (client->started())
                client->close();
            break;
        }
        else if (line == "started")
        {
            std::cout << "started: " << client->started() << std::endl;
            continue;
        }
        else if (line == "add_user")
        {

//            if (client->started()){
//                client->send("");
//            }

            continue;
        }
        else if (line == "add_user")
        {
            if (client->started()){

                std::string msg = client->create_message(
                        boost::lexical_cast<std::string>(boost::uuids::random_generator()())
                        , "add_user"
                        , "test"
                        ,""
                        ,""
                        ,""
                        ,client->get_hash("test", "test")
                        ,"add_user"
                        , "admin"
                        );
                client->send("cmd " + msg, false);

            }
            continue;
        }
        else if (line == "join_channel")
        {
            if (client->started()){
                std::cout << "get uuid channel ...." << std::endl;

                std::string _uuid_channel;
                getline(std::cin, _uuid_channel);
                if (_uuid_channel.empty())
                    continue;

                client->subscribe_to_channel(_uuid_channel, "00000000-0000-0000-0000-000000000000");

//                std::string msg = client->create_message(
//                        boost::lexical_cast<std::string>(boost::uuids::random_generator()())
//                        , "subscribe_to_channel"
//                        , "admin"
//                        ,_uuid_sub
//                        ,""
//                        ,""
//                        ,""
//                        ,"subscribe_to_channel"
//                        , ""
//                        );
//                client->send("cmd " + msg, false);

            }
            continue;
        }
        else if (line == "set_param")
        {
            if (client->started()){
                client->set_session_param(arc_json::uuid_to_string(client->get_uuid()), "admin", "admin", "Console", arc_json::uuid_to_string(client->get_user_uuid()));
            }
        }
        else
        {
            if (client->started())
                client->send(line, true);
            continue;
        }

    }

    f.join();



    //    boost::asio::thread_pool pool(4);
    //    boost::asio::post(pool, start);
    //    boost::asio::post(pool,
    //                      []()
    //                      {
    //        //
    //                      });

    // Wait for all tasks in the pool to complete.
    //   pool.join();
    return EXIT_SUCCESS;
}


