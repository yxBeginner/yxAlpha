// yx
#ifndef YXALPHA_INETADDR_H_
#define YXALPHA_INETADDR_H_

// #include<arpa/inet.h>
#include <netinet/in.h>
#include <string>

namespace yxalp {

class InetAddr {
public:
    // 通常为 server socket 构造方式
    explicit InetAddr(uint16_t port);
    InetAddr(const std::string &ip, uint16_t port);
    // 通常是 client socket 的构造方式
    explicit InetAddr(const sockaddr_in &addr) : addr_(addr) {}
    // default
    InetAddr(const InetAddr &) = default;
    InetAddr & operator= (const InetAddr &) = default;
    // ~InetAddr();  // TODO

    const struct sockaddr_in & get_addr() const { return addr_; }
    void set_addr(const struct sockaddr_in &addr) { addr_ = addr; };
private:
    struct sockaddr_in addr_;
};

}  // yxalp

# endif  // YXALPHA_INETADDR_H_