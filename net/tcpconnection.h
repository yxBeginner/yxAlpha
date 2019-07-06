// yx
#ifndef YXALPHA_TCPCONNECTION_H_
#define YXALPHA_TCPCONNECTION_H_

#include <memory>

#include "net/inetaddr.h"
#include "callback.h"

namespace yxalp {

class Dispatcher;
class EventHandler;
class Socket;

class TcpConnection : std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(Dispatcher *dispatcher, Socket &&socket, 
                                     const InetAddr &addr, const std::string &conn_name);
    ~TcpConnection();
    TcpConnection& operator=(const TcpConnection &) = delete;
    TcpConnection(const TcpConnection &) = delete;

    const std::string & name() { return conn_name_; }
    bool is_connected() const { return state_ == CONNECTED; }

    // 用户与 TCPServer 都可以设置 CallBack
    void set_connection_call_back(const ConnectionCallback &cb) 
    { connection_call_back_ = cb; }
    void set_message_call_back(const MessageCallback &cb)
    { message_call_back_ = cb; }
    // 连接已建立, 只应该由 server 设置.
    void set_connection_established();
    // 连接已断开, 此时TCPServer 与 TcpClient "已经"移除自己持有的对象
    void set_connection_destroyed();

    // 内部使用, 只应该由 server 调用删除
    void set_close_call_back(const CloseCallBack & cb)
    { close_call_back_ = cb; }

private:
    enum ConnState {CONNECTING, CONNECTED, DISCONNECTED};

    void set_state(ConnState cs) { state_ = cs; }
    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleClose();

    Dispatcher *dispatcher_;
    ConnState state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<EventHandler> event_handler_;  // 用于到 Dispatcher 上注册监听的 Handler
    InetAddr addr_;
    std::string conn_name_;
    ConnectionCallback connection_call_back_;
    MessageCallback message_call_back_;
    CloseCallBack close_call_back_;
};

}  // namespace yxalp

#endif  // YXALPHA_TCPCONNECTION_H_