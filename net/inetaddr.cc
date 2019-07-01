// yx
#include "inetaddr.h"
#include <cstring>

namespace yxalp {
    InetAddr::InetAddr(uint16_t port) {
        std::memset(&addr_, 0, sizeof (addr_));
        addr_.sin_family = AF_INET;
        // addr_.sin_addr.s_addr = inet_addr(serv_ip);
        addr_.sin_addr.s_addr = htonl(INADDR_ANY);
        addr_.sin_port = htons(port);
    }

    InetAddr::InetAddr(const std::string &ip, uint16_t port) {
        std::memset(&addr_, 0, sizeof (addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = htonl(std::stoi(ip.c_str()));  // or inet_addr() ?
        addr_.sin_port = htons(port);
    }

}