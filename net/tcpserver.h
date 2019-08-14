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
class DispatcherPool;

class TcpServer {
public:
    TcpServer(Dispatcher *dispatcher, const InetAddr &server_addr);
    ~TcpServer();
    TcpServer & operator=(const TcpServer &) = delete;
    TcpServer(const TcpServer &) = delete;

    // num_threads = 0, 将不会创建任何 SubReactor
    // num_threads = n, 将创建 n 个 SubReactor 线程
    void SetThreadNum(int num_threads);

    // 必须在 start 之前 Call SetThreadNum.
    void Start();

    // 为 server 上的每个连接设置 CallBack, 创建新连接时默认设置.
    void set_connection_call_back(const ConnectionCallback &cb) 
    { connection_call_back_ = cb; }
    void set_message_call_back(const MessageCallback &cb) 
    { message_call_back_ = cb; }
    void set_write_complete_callback(const WriteCompleteCallback &cb)
    { write_complete_callback_ = cb; }

    Dispatcher* get_dispatcher() const { return dispatcher_; }
    //TODO setsockopt
private:
    // 在Acceptor::EventHandlerCallBack() 中被回调, 也即 accept 新连接时.
    void NewConnection(Socket &&sock, const InetAddr &client_addr);
    // 在 TcpConnection 的 HandleClose 中被调用
    void RemoveConnection(const TcpConnectionPtr &conn);
    void RemoveConnectionInLoop(const TcpConnectionPtr &conn);

    const std::string name_;
    Dispatcher * dispatcher_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<DispatcherPool> dispatcher_pool_;
    ConnectionCallback connection_call_back_;
    MessageCallback message_call_back_;
    WriteCompleteCallback write_complete_callback_;
    bool started_;
    int conn_id_;
    std::map<std::string, TcpConnectionPtr> connections_;
};

}  // namespace yxalp

#endif  // YXALPHA_TCPSERVER_H_