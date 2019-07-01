// yx
#ifndef YXALPHA_SOCKET_H_
#define YXALPHA_SOCKET_H_

namespace yxalp {

class InetAddr;

// 对一个 socket fd 的 wrapper, 每个 socket 只由一个 IO 线程负责
// 负责一个 socket 的完整生命期
class Socket {
public:
    explicit Socket(int fd) : fd_(fd) { }
    explicit Socket(const Socket && sock);  // 接受右值构造
    Socket(const Socket &) = delete;
    Socket& operator=(const Socket &) = delete;
    ~Socket();

    // poll 与 read / write
    int fd() const { return fd_; }
    void Bind(const InetAddr &addr);
    void Listen();
    // 与 accept() 类似, 成功时会返回 socket file descriptor
    int Accept(InetAddr &con_addr);
    void enable_reuse_addr();
    void disable_reuse_addr();
    // nodelay ...
private:
    int fd_;
    // InetAddr 不直接设置地址只是为了让 server socket 方便绑定地址吗, 要不然 C/S 分开吧
};

}  // namespace yxalp

#endif  // YXALPHA_SOCKET_H_