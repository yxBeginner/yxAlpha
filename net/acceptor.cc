// yx
#include "acceptor.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "inetaddr.h"
#include "logger/logging.h"

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
      event_handler_(server_socket_.fd(), dispatcher_) {
    server_socket_.enable_reuse_addr();
    server_socket_.Bind(addr);
    event_handler_.set_read_func(std::bind(&Acceptor::EventHandlerCallBack, this));
    DLOG << "Acceptor::Acceptor() : Constructor";
}

Acceptor::~Acceptor() {}

void Acceptor::Listen() {
    listenning_ = true;
    server_socket_.Listen();
    event_handler_.set_care_read();
    DLOG << "Acceptor::Listen() : server socket start listening.";
}

void Acceptor::EventHandlerCallBack() {
    InetAddr client_addr {0};  // TODO
    // TODO, 可选连续多次 accept, 尝试连接多个客户端
    int connfd = server_socket_.Accept(client_addr);
    Socket sock(connfd);  // 多此一举 ?addr 也使用 move ?
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            // newConnectionCallback_(connfd, client_addr);
            DLOG << "Acceptor::EventHandlerCallBack()";
            newConnectionCallback_(std::move(sock), client_addr);  // 局部 Socket 对象失效
        } else {
            LOG << "Acceptor::EventHandlerCallBack() : accept socket fd < 0";
            close(connfd);
        }
    }
}

}  // namespace yxalp