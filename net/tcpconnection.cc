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
      message_call_back_(nullptr),
      input_buf_(),
      output_buf_() {
    
    event_handler_->set_read_func(std::bind(&TcpConnection::HandleRead, this));  // 设置 Handler 上的 CallBack
    event_handler_->set_write_func(std::bind(&TcpConnection::HandleWrite, this));
    event_handler_->set_error_func(std::bind(&TcpConnection::HandleWrite, this));
    event_handler_->set_close_func(std::bind(&TcpConnection::HandleWrite, this));
    DLOG << "TcpConnection::TcpConnection : fd " << socket_->fd() 
                 << "name : " << conn_name_.c_str();
}

TcpConnection::~TcpConnection() {
    // log close, 注意 socket 才是真正管理 fd 的
    DLOG << "TcpConnection::~TcpConnection(), fd : " << socket_->fd()
              << "name : " << conn_name_.c_str();
}

void TcpConnection::ShutDown() {
    // 其余状态不必考虑, CONNECTING 客端还没持有本连接, 已经处于
    // DISCONNECTING 时, 不必再次设置.
    // DISCONNECTED 状态则更不必考虑, 因为本连接已经失效
    if (state_ == CONNECTED) {
        state_ = DISCONNECTING;
        // 我们需要到本 Connection 所在的线程去 shutdown
        dispatcher_->RunInLoop(std::bind(
                                    &TcpConnection::ShutDownInLoop, this));
    }
}

void TcpConnection::ShutDownInLoop() {
    dispatcher_->AssertInLoopThread();
    if (!(event_handler_->is_care_write())) {
        socket_->ShutDownWrite();
    }
}

// 保证线程安全
void TcpConnection::Send(const std::string &str) {
    if (state_ == CONNECTED) {
        if (dispatcher_->is_same_thread()) {
            SendInLoop(str);
            DLOG << "TcpConnection::Send() : SendInLoop(str);";
        } else {
            // 调用 send 者不在当前线程, 则转由 EventLoop 的线程调用 sendInLoop
            dispatcher_->RunInLoop(std::bind(&TcpConnection::SendInLoop, this, str));
            DLOG << "TcpConnection::Send() : RunInLoop";
        }
    }
}

void TcpConnection::SendInLoop(const std::string &str) {
    dispatcher_->AssertInLoopThread();
    ssize_t n = 0;
    // 尝试直接写
    if (!(event_handler_->is_care_write()) && output_buf_.payload_size() == 0) {
        n = write(event_handler_->fd(),str.data(), str.size());  // TODO writeV
        if (n >= 0) {
            if (static_cast<size_t> (n) < str.size()) {
                DLOG << "TcpConnection::SendInLoop(), fd : " << socket_->fd()
                << "name : " << conn_name_.c_str() << "There are more data to write";
            }
        } else {
            n = 0;
            if (errno != EAGAIN) {
                DLOG << "TcpConnection::SendInLoop(), fd : " << socket_->fd()
                << "name : " << conn_name_.c_str() << "Error in write";
            } else {
                // 到这里就是发生了 EAGAIN, 如果应用户层缓冲区是空的, 但内核缓冲区又是
                // 满的就会发生这种状态. 这要么是应用层数据刷的太快, 要么就是网络状态太
                // 差. 此时应该做的是写到 Buffer 中, 它会自动增长.
            }
        }
    }

    if (static_cast<size_t> (n) < str.size()) {
        output_buf_.Append(str.data()+n, str.size()-n);
        if (!(event_handler_->is_care_write())) {
            event_handler_->set_care_write();
        }
    }
}


void TcpConnection::set_connection_established() {
    dispatcher_->AssertInLoopThread();  // IO thread
    DLOG << "TcpConnection::set_connection_established(), fd : " << socket_->fd()
              << " name : " << conn_name_.c_str();
    assert(state_ == CONNECTING);
    state_ = CONNECTED;
    event_handler_->set_care_read();
    connection_call_back_(shared_from_this());  // 回调用户函数
}

void TcpConnection::set_connection_destroyed() {
    dispatcher_->AssertInLoopThread();
    DLOG << "TcpConnection::set_connection_destroyed(), fd : " << socket_->fd()
              << "name : " << conn_name_.c_str();
    assert(state_ == CONNECTED || state_ == DISCONNECTING);
    state_ =  DISCONNECTED;
    event_handler_->set_stop_care_all();  // 没有维持半关闭状态 TODO
    connection_call_back_(shared_from_this());   // NOT necessary

    dispatcher_->RemoveEventHandler(event_handler_.get());
}

void TcpConnection::HandleRead() {
    DLOG << "TcpConnection::HandleRead(), fd : " << socket_->fd()
              << "name : " << conn_name_.c_str();
    int saved_errno = 0;
    ssize_t n = input_buf_.readFd(event_handler_->fd(), &saved_errno);
    if (n > 0) {
        message_call_back_(shared_from_this(), &input_buf_);  // 回调用户的 CallBack
    } else if (n == 0) {
        errno = saved_errno;
        HandleClose();  // EOF
    } else {
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    if (event_handler_->is_care_write()) {
        ssize_t n = write(event_handler_->fd(), 
                                          output_buf_.peek(), output_buf_.payload_size());
        if (n>0) {
            output_buf_.move_read_index(n);
            if (output_buf_.payload_size() == 0) {
                // buffer 已空, 不在监听写事件, 避免 busy Loop
                event_handler_->set_stop_care_write();
                if (state_ == DISCONNECTING) {  // 本端 FIN
                    ShutDownInLoop();  // 回调肯定在本线程之内
                }
            } else {
                DLOG << "TcpConnection::HandleWrite(), fd : " << socket_->fd()
                << "name : " << conn_name_.c_str() << "There are more data to write";
            }
        } else {
            LOG << "TcpConnection::HandleWrite(), fd : " << socket_->fd()
            << "name : " << conn_name_.c_str() << "Writing error.";
        }
    } else {
        // 如果运行到此处是编程逻辑出现错误
        DLOG << "TcpConnection::HandleWrite(), fd : " << socket_->fd()
                        << "name : " << conn_name_.c_str() << "logic error";
    }
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
                  << " - SO_ERROR -  : " << optval;
    }
}

// 不能在此直接销毁自身, 解注册 Dispatcher, 需要回调 TCPServer 的 close
// server 上持有本对象的 shared_ptr
void TcpConnection::HandleClose() {
    LOG << "TcpConnection::HandleClose(), fd" << socket_->fd()
              << "name : " << conn_name_.c_str()
              << " with state: " << state_;
    assert(state_ == CONNECTED || state_ == DISCONNECTING);
    // 不在关注任何事件, 执行完 server 的回调之后, 本对象将不复存在
    event_handler_->set_stop_care_all();
    close_call_back_(shared_from_this());
}

} // namespace yxalp
