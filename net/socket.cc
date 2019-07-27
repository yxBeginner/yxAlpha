// yx
#include "socket.h"

#include  <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>

#include "inetaddr.h"
#include "logger/logging.h"

namespace yxalp {

Socket::Socket(Socket && rhs) {
    this->fd_ = rhs.fd_;
    this->is_valid_ = true;
    rhs.is_valid_ = false;  // 被移动者的 fd 无效
}

Socket::~Socket() {
    DLOG << "Socket : ~Socket(): before close fd : " << fd_;
    if (is_valid_) {
        int ret = close(fd_);
        if (ret < 0) {
            LOG << "Socket : ~Socket() : Error";
        }
    } else {
        DLOG << "Socket : ~Socket(): close fd : " << fd_ << "无效的 fd";
    }
}

void Socket::Bind(const InetAddr &addr) {
    // int bind(int __fd, const sockaddr *__addr, socklen_t __len)
    int ret = bind(fd_, (const struct sockaddr*) (&(addr.get_addr())), sizeof (struct sockaddr_in));  // or reinterpret ?
    if (ret < 0) {
        LOG << "Socket : bind() : can't bind socket";
        abort();
    }
}

void Socket::Listen() {
    /* Maximum queue length specifiable by listen.  */
    int ret = listen(fd_, SOMAXCONN);  // default
    if (ret < 0) {
        LOG << "Socket : listen() : Error ";
        abort();
    }
}

// 与 accept() 类似, 成功时会返回 socket file descriptor
int Socket::Accept(InetAddr &con_addr) {
    int connfd;
    socklen_t client_len;
    struct sockaddr_in client_addr {0};
    client_len = sizeof(client_addr);
    // 返回一个非阻塞的 fd.
    connfd = accept4(fd_,  (sockaddr *) &client_addr, &client_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        int saved_errno = errno;
        LOG << "Socket : accept() : Error";
        switch (saved_errno) {
            case EAGAIN:
            case EINTR:
            case EPERM:
            case EMFILE:
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                LOG << "Socket: accept(): Fatal Error" << saved_errno;
                abort();
                break;
            default:
                LOG << "Socket: accept(): Unknown Error"  << saved_errno;
                abort();
                break;
        }
    } else {
        con_addr.set_addr(client_addr);
    }
    return connfd;
}

void Socket::ShutDownWrite() {
    if (shutdown(fd_, SHUT_WR) == -1) {
        LOG << "Socket::ShutDown() : Error";
    }
}

void Socket::enable_reuse_addr() {
    int val = 1;  // s
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));  // |SO_REUSEPORT
}

void Socket::disable_reuse_addr() {
    int val = 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));  // |SO_REUSEPORT
}

void Socket::enable_tcp_nodelay() {
    int val = 1;
    setsockopt(fd_, IPPROTO_IP, TCP_NODELAY, &val, sizeof (val));
}

void Socket::disable_tcp_nodelay() {
    int val = 0;
    setsockopt(fd_, IPPROTO_IP, TCP_NODELAY, &val, sizeof (val));
}

void Socket::enable_keep_alive() {
    int val = 1;
    int ret = setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof (val));
    if (ret < 0) {  // TODO better
        LOG << "Socket::enable_keep_alive() : Error";
        abort();
    }
}

void Socket::disable_keep_alive() {
    int val = 0;
    int ret = setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof (val));
    if (ret < 0) {  // TODO better
        LOG << "Socket::disable_keep_alive() : Error";
        abort();
    }
}

void Socket::enable_reuse_port() {
    int val = 1;
    int ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &val, 
                         static_cast<socklen_t> (sizeof (val)));
    if (ret < 0) {
        LOG << "Socket::enable_reuse_port : Error";
        abort();
    }
}

void Socket::disable_reuse_port() {
    int val = 0;
    int ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &val, 
                         static_cast<socklen_t> (sizeof (val)));
    if (ret < 0) {
        LOG << "Socket::disable_reuse_port : Error";
        abort();
    }
}

}  // yxalp
