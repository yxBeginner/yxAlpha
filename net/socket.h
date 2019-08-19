// yx
#ifndef YXALPHA_SOCKET_H_
#define YXALPHA_SOCKET_H_

namespace yxalp {

class InetAddr;

// 每个 socket 应当只由一个 IO 线程负责
// 对一个 socket fd 的 wrapper, 负责一个 socket fd 的完整生命期
class Socket {
public:
    explicit Socket(int fd) : fd_(fd), is_valid_(true) { }
    // explicit Socket(Socket && rhs);  // 接受右值构造
    Socket(const Socket &) = delete;
    Socket& operator=(const Socket &) = delete;
    // Socket(Socket const &&);
    ~Socket();

    // poll 与 read / write
    int fd() const { return fd_; }
    void Bind(const InetAddr &addr);
    void Listen();
    // 与 accept() 类似, 成功时会返回 socket file descriptor
    int Accept(InetAddr &con_addr);
    void ShutDownWrite();
    void enable_reuse_addr();
    void disable_reuse_addr();
    void enable_reuse_port();
    void disable_reuse_port();
    void enable_tcp_nodelay();
    void disable_tcp_nodelay();
    void enable_keep_alive();
    void disable_keep_alive();

private:
    int fd_;
    bool is_valid_;
    // InetAddr 不直接设置地址只是为了让 server socket 方便绑定地址, 要不然 C/S 分开吧
};

}  // namespace yxalp

#endif  // YXALPHA_SOCKET_H_