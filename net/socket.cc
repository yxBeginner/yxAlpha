// yx
#include "socket.h"
#include  <unistd.h>

#include "inetaddr.h"

namespace yxalp {

Socket::~Socket() {
    close(fd_);
}

void Socket::Bind(const InetAddr &addr) {
    // int bind(int __fd, const sockaddr *__addr, socklen_t __len)
    int ret = bind(fd_, (const struct sockaddr*) (&(addr.get_addr())), sizeof (struct sockaddr_in));  // or reinterpret ?
    if (ret < 0) {
        // log " Socket : bind() : can't bind socket"
    }
}

void Socket::Listen() {
    /* Maximum queue length specifiable by listen.  */
    int ret = listen(fd_, SOMAXCONN);  // default
    if (ret < 0) {
        // log " Socket : listen() : reason "
    }
}

// 与 accept() 类似, 成功时会返回 socket file descriptor
int Socket::Accept(InetAddr &con_addr) {
    int connfd;
    socklen_t client_len;
    struct sockaddr_in client_addr;  // 需要全部设置为 0 吗?
    client_len = sizeof(client_addr);
    // 返回一个非阻塞的 fd.
    connfd = accept4(fd_,  (sockaddr *) &client_addr, &client_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        // log "Socket : accept() : reason"
        // 应当区分不同的情况.
    } else {
        con_addr.set_addr(client_addr);
    }
    return connfd;  // 直接做成返回 Socket呢?
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