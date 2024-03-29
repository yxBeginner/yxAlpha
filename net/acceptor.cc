// yx
#include "acceptor.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "inetaddr.h"
#include "logger/logging.h"

// 为长连接优化
// #define LONGCONN

namespace yxalp {

static int CreateServerSocket() {
    // SO_REUSEADDR is optional
    int server_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 
                             IPPROTO_TCP);
    if (server_fd < 0) {
        LOG << "CreateServerSocket() : Error in creating server socket.";
        abort();
    }
    return server_fd;
}

Acceptor::Acceptor(Dispatcher *dispatcher, const InetAddr &addr)
    : dispatcher_(dispatcher),
      server_socket_(CreateServerSocket()),
      event_handler_(server_socket_.fd(), dispatcher_),
      listenning_(false),
      idle_fd_(open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    server_socket_.enable_reuse_addr();
    server_socket_.enable_tcp_nodelay();
    // server_socket_.enable_keep_alive();
    server_socket_.enable_reuse_port();
    server_socket_.Bind(addr);
    event_handler_.set_read_func(std::bind(&Acceptor::EventHandlerCallBack, this));
    DLOG << "Acceptor::Acceptor() : Constructor";
}

Acceptor::~Acceptor() {
    event_handler_.set_stop_care_all();
    close(idle_fd_);
}

void Acceptor::Listen() {
    listenning_ = true;
    server_socket_.Listen();
    event_handler_.set_care_read();
}

void Acceptor::EventHandlerCallBack() {
    InetAddr client_addr {0};  // TODO
    // TODO, 可选连续多次 accept, 尝试连接多个客户端
    // int connfd = server_socket_.Accept(client_addr);
#ifdef LONGCONN
    int connfd = server_socket_.Accept(client_addr);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            // newConnectionCallback_(connfd, client_addr);
            DLOG << "Acceptor::EventHandlerCallBack()";
            newConnectionCallback_(connfd, client_addr);
        } else {
            // unlikely
            close(connfd);
        }
    } else {
        LOG << "Acceptor::EventHandlerCallBack() : accept socket fd < 0";
        if (errno == EMFILE) {
            close(idle_fd_);
            idle_fd_ = accept(server_socket_.fd(), NULL, NULL);
            close(idle_fd_);
            idle_fd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
#else
    int connfd;
    while((connfd = server_socket_.Accept(client_addr))>0) {
        if (newConnectionCallback_) {
            // newConnectionCallback_(connfd, client_addr);
            DLOG << "Acceptor::EventHandlerCallBack()";
            newConnectionCallback_(connfd, client_addr);
        } else {
            // unlikely
            close(connfd);
        }
    }
    if (connfd < 0) {
        LOG << "Acceptor::EventHandlerCallBack() : accept socket fd < 0";
        if (errno == EMFILE) {
            close(idle_fd_);
            idle_fd_ = accept(server_socket_.fd(), NULL, NULL);
            close(idle_fd_);
            idle_fd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
#endif
}

}  // namespace yxalp