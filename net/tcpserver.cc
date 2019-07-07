// yx
#include "tcpserver.h"

#include <utility>

#include "socket.h"
#include "inetaddr.h"
#include "logger/logging.h"
#include "acceptor.h"
#include "reactor/dispatcher.h"

namespace yxalp {

TcpServer::TcpServer(Dispatcher *dispatcher, const InetAddr &server_addr)
    : name_(server_addr.get_iner_host_port()),  // TODO 修正为 ip : port
      dispatcher_(dispatcher),
      acceptor_(std::make_unique<Acceptor>(dispatcher, server_addr)),
      connection_call_back_(nullptr),
      message_call_back_(nullptr),
      started_(false),
      conn_id_(0),
      connections_() {
          acceptor_->set_new_connection_callback(
              std::bind(&TcpServer::NewConnection, this, 
                                  std::placeholders::_1, std::placeholders:: _2));

}

TcpServer::~TcpServer() {
    // TODO close all Connection
    LOG << "TcpServer::~TcpServer() : destructor [" << name_.c_str() << "]";
}

void TcpServer::Start() {
    LOG << "TcpServer::Start() [" << name_.c_str() << "]";
    if (!started_) {
        started_ = true;
    }
    if (!(acceptor_->is_listenning())) {
        // 将 listen 的启动放到 Dispatcher 的事件循环中(注意, 因为 listen 者可能并不
        // 处于主线程中) get() 只用于指定地址
        dispatcher_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_.get()));
    }
}

void TcpServer::NewConnection(Socket &&sock, const InetAddr &client_addr) {
    dispatcher_->AssertInLoopThread();  // 只可以在从 IO 线程中创建
    ++conn_id_;
    std::string conn_name = name_ + std::to_string(conn_id_);
    LOG << "TcpServer::NewConnection() [" << name_.c_str()  << "] - new connection ["
              << conn_name.c_str() << "] ip:port " << client_addr.get_iner_host_port().c_str();
    TcpConnectionPtr conn = std::make_shared<TcpConnection> (dispatcher_, 
                                                         std::move(sock), client_addr, conn_name);  // hold 1
    // 更新 map
    connections_.insert(std::pair<std::string, TcpConnectionPtr> (conn_name, conn));  // hold 1
    // DLOG << "TcpServer::NewConnection() [" << name_.c_str()  << "] - new connection ["
    //              << conn_name.c_str() << "] holds : " << connections_[conn_name].use_count();
    conn->set_connection_call_back(connection_call_back_);
    conn->set_message_call_back(message_call_back_);
    conn->set_close_call_back(
                    std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    conn->set_connection_established();
}  // release 1

void TcpServer::RemoveConnection(const TcpConnectionPtr &conn) {
    const std::string &conn_name = conn->get_name();
    LOG << "TcpServer::RemoveConnection [" << name_.c_str()  << "] - new connection ["
              << conn_name.c_str() << "] ip:port " << conn->get_addr().get_iner_host_port().c_str();
    size_t ret = connections_.erase(conn_name);
    assert(ret == 1);
    // 最后此处的 bind, 
    dispatcher_->QueueInLoop(
                                std::bind(&TcpConnection::set_connection_destroyed, conn));
}

}  // namespace yxalp 
