// yx
#ifndef YXALPHA_TCPSERVER_H_
#define YXALPHA_TCPSERVER_H_

#include "callback.h"
#include "tcpconnection.h"

namespace yxalp {

class Acceptor;
class Dispatcher;

class TcpServer {
public:
    // TcpServer(Dispatcher *dispatcher, const  );
    ~TcpServer();
    TcpServer & operator=(const TcpServer &) = delete;
    TcpServer(const TcpServer &) = delete;

    void Start();

    // 为 server 上的每个连接设置 CallBack, 创建新连接时默认设置.
    void set_connection_call_back(const ConnectionCallback &cb) { connection_call_back_ = cb; }
    void set_message_call_back(const MessageCallback &cb) { message_call_back_ = cb; }

private:
    // 在Acceptor::EventHandlerCallBack() 中被回调, 也即 accept 新连接时.
    void NewConnection(Socket sock, const InetAddr &client_addr);

    Dispatcher * dispatcher_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connection_call_back_;
    MessageCallback message_call_back_;

    bool started_;

};

}  // namespace yxalp

#endif  // YXALPHA_TCPSERVER_H_