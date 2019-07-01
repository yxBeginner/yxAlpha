// yx
#include "acceptor.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "inetaddr.h"

namespace yxalp {

static int CreateServerSocket() {
    int server_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 
                                              IPPROTO_TCP);
    if (server_fd < 0) {
        // log error in create server socket.
        // abort();
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
}

void Acceptor::Listen() {
    server_socket_.Listen();
    event_handler_.set_care_read();
}

void Acceptor::EventHandlerCallBack() {
    InetAddr client_addr(0);  // TODO bad design
    int connfd = server_socket_.Accept(client_addr);  // 或者连续多次 accept, 尝试连接多个客户端
    Socket sock(connfd);  // 多此一举 ?addr 也使用 move ?
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            // newConnectionCallback_(connfd, client_addr);
            newConnectionCallback_(std::move(sock), client_addr);  // 局部 Socket 对象失效
        } else {
            close(connfd);  
        }
    }
}

}  // namespace yxalp