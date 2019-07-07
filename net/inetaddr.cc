// yx
#include "inetaddr.h"
#include <cstring>
#include <fcntl.h>
#include<arpa/inet.h>

namespace yxalp {

InetAddr::InetAddr(uint16_t port) {
    std::memset(&addr_, 0, sizeof (addr_));
    addr_.sin_family = AF_INET;
    // addr_.sin_addr.s_addr = inet_addr(serv_ip);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);
}

InetAddr::InetAddr(const std::string &ip, uint16_t port) {
    set_host_port(ip.c_str(), port, addr_); 
}

std::string InetAddr::get_iner_host_port() const{
    char buf[32];
    get_host_port(buf, sizeof (buf), addr_);
    return std::string(buf);
}

void InetAddr::get_host_port(char *buf, size_t len, const struct sockaddr_in &addr) {
    char host[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
    uint16_t port = ntohs(addr.sin_port);
    snprintf(buf, len, "%s:%u", host, port);
}

void InetAddr::set_host_port(const char *ip, uint16_t port, struct sockaddr_in &addr) {
    std::memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(std::atoi(ip));  // or inet_addr() ?
    addr.sin_port = htons(port);
}

}