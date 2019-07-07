// yx
#include "socket.h"

#include  <unistd.h>
#include <errno.h>

#include "inetaddr.h"
#include "logger/logging.h"

namespace yxalp {

Socket::Socket(Socket && rhs) {
    this->fd_ = rhs.fd_;
    rhs.is_valid_ = false;  // 被移动者的 fd 无效
}

Socket::~Socket() {
    if (is_valid_) {
        int ret = close(fd_);
        if (ret < 0) {
            LOG << "Socket : close() : Error";
        }
    } else {
        // 无效的 fd
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
    return connfd;  // TODO 直接做成返回 Socket
}

void Socket::enable_reuse_addr() {
    int val = 1;  // 1 s
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));  // |SO_REUSEPORT
}
// 需要验证.
void Socket::disable_reuse_addr() {
    int val = 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));  // |SO_REUSEPORT
}

}  // yxalp
