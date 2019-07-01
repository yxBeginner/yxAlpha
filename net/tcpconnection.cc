// yx
#include "tcpconnection.h"

#include <cassert>
#include <unistd.h>

#include "socket.h"
#include "reactor/eventhandler.h"
#include "reactor/dispatcher.h"

namespace yxalp {

TcpConnection::TcpConnection(Dispatcher *dispatcher, Socket socket, const InetAddr &addr)
    : dispatcher_(dispatcher),
      state_(CONNECTING),
      socket_(std::make_unique<Socket> (std::move(socket))),  // hold on
      event_handler_(std::make_unique<EventHandler> (socket_->fd(), dispatcher_)),
      addr_(addr),
      connection_call_back_(nullptr),
      message_call_back_(nullptr) {
    
    event_handler_->set_read_func(std::bind(&TcpConnection::HandleRead, this));  // 设置 Handler 上的 CallBack
}

TcpConnection::~TcpConnection() {
    // log close, 注意 socket 才是真正管理 fd 的
}

void TcpConnection::set_connection_established() {
    // log ?
    assert(state_ == CONNECTING);
    state_ = CONNECTED;
    event_handler_->set_care_read();
    connection_call_back_(shared_from_this());
}

void TcpConnection::HandleRead() {
    char buf[65536];
    ssize_t n = ::read(event_handler_->fd(), buf, sizeof buf);
    // if (n <= 0) close(fd);
    message_call_back_(shared_from_this(), buf, n);  // 回调用户的 CallBack
}
    
} // namespace yxalp


