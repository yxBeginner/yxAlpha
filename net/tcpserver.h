// yx
#ifndef YXALPHA_TCPSERVER_H_
#define YXALPHA_TCPSERVER_H_

#include <string>
#include <map>

#include "callback.h"
#include "tcpconnection.h"

namespace yxalp {

class Acceptor;
class Dispatcher;

class TcpServer {
public:
    TcpServer(Dispatcher *dispatcher, const InetAddr &server_addr);
    ~TcpServer();
    TcpServer & operator=(const TcpServer &) = delete;
    TcpServer(const TcpServer &) = delete;

    void Start();

    // 为 server 上的每个连接设置 CallBack, 创建新连接时默认设置.
    void set_connection_call_back(const ConnectionCallback &cb) { connection_call_back_ = cb; }
    void set_message_call_back(const MessageCallback &cb) { message_call_back_ = cb; }

    //TODO setsockopt
private:
    // 在Acceptor::EventHandlerCallBack() 中被回调, 也即 accept 新连接时.
    void NewConnection(Socket &&sock, const InetAddr &client_addr);
    // 在 TcpConnection 的 HandleClose 中被调用
    void RemoveConnection(const TcpConnectionPtr &conn);

    const std::string name_;
    Dispatcher * dispatcher_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connection_call_back_;
    MessageCallback message_call_back_;
    bool started_;
    int conn_id_;
    // Warn: 维护一个所有 Connection 的map, 这与 selector 并不一致, 后者维护的是存活的 fd.
    std::map<std::string, TcpConnectionPtr> connections_;
};

}  // namespace yxalp

#endif  // YXALPHA_TCPSERVER_H_