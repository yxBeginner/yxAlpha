// yx
#include "tcpconnection.h"

#include <cassert>
#include <unistd.h>

#include "socket.h"
#include "reactor/eventhandler.h"
#include "reactor/dispatcher.h"
#include "logger/logging.h"

namespace yxalp {

TcpConnection::TcpConnection(Dispatcher *dispatcher, Socket &&socket, 
                                                                    const InetAddr &addr, const std::string &conn_name)
    : dispatcher_(dispatcher),
      state_(CONNECTING),
      socket_(std::make_unique<Socket> (std::move(socket))),  // hold on
      event_handler_(std::make_unique<EventHandler> (socket_->fd(), dispatcher_)),
      addr_(addr),
      conn_name_(conn_name),
      connection_call_back_(nullptr),
      message_call_back_(nullptr) {
    
    event_handler_->set_read_func(std::bind(&TcpConnection::HandleRead, this));  // 设置 Handler 上的 CallBack
}

TcpConnection::~TcpConnection() {
    // log close, 注意 socket 才是真正管理 fd 的
    LOG << "TcpConnection::~TcpConnection(), fd : " << socket_->fd();
}

void TcpConnection::set_connection_established() {
    // log ?
    assert(state_ == CONNECTING);
    state_ = CONNECTED;
    event_handler_->set_care_read();
    connection_call_back_(shared_from_this());  // 回调用户函数
}

void TcpConnection::set_connection_destroyed() {
    assert(state_ == CONNECTED);
    state_ =  DISCONNECTED;
    event_handler_->set_stop_care_all();  // 没有维持半关闭状态 TODO
    connection_call_back_(shared_from_this());   // NOT necessary

    dispatcher_->RemoveEventHandler(event_handler_.get());
}

void TcpConnection::HandleRead() {
    char buf[65536];
    ssize_t n = ::read(event_handler_->fd(), buf, sizeof buf);
    if (n > 0) {
        message_call_back_(shared_from_this(), buf, n);  // 回调用户的 CallBack
    } else if (n == 0) {
        HandleClose();
    } else {
        HandleError();
    }
}

void TcpConnection::HandleWrite() {

}

// TODO log actual info
void TcpConnection::HandleError() {
    int optval;
    socklen_t optlen = sizeof (optval);
    int ret = getsockopt(socket_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if (ret < 0) {
        int err = errno;
        LOG << "TcpConnection::HandleError(), fd : " << socket_->fd() 
                  << " - SO_ERROR - errno : " << err;
    } else {
        LOG << "TcpConnection::HandleError(), fd : " << socket_->fd()
                  << " - SO_ERROR -  : " << ret;
    }
}

// 不能在此直接销毁自身, 解注册 Dispatcher, 需要回调 TCPServer 的 close
void TcpConnection::HandleClose() {
    LOG << "TcpConnection::HandleClose(), fd" << socket_->fd() 
              << " with state: " << state_;
    assert(state_ == CONNECTED);
    // 不在关注任何事件, 执行完 server 的回调之后, 本对象将不复存在
    event_handler_->set_stop_care_all();
    close_call_back_(shared_from_this());
}

} // namespace yxalp


