// g_test_fix_bad_weak_ptr

#include "../net/tcpserver.h"
#include "../reactor/dispatcher.h"
#include "../net/inetaddr.h"
#include "../net/socket.h"
#include "reactor/eventhandler.h"
#include "reactor/dispatcher.h"
#include "logger/logging.h"
#include <stdio.h>


void onConnection(const yxalp::TcpConnectionPtr& conn) {
    printf("onConnection(): shared ptr holder : %ld \n", conn.use_count());
    yxalp::TcpConnectionPtr local_ptr = conn;
    if (conn->is_connected()) {
        printf("onConnection(): new connection [%s] from\n",
            conn->get_name().c_str());
            // conn-> ->peerAddress().toHostPort().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n",
            conn->get_name().c_str());
    }
}

void onMessage(const yxalp::TcpConnectionPtr& conn,
                                    const char* data, ssize_t len) {
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         len, conn->get_name().c_str());
}

int main() {
    using namespace yxalp;
    yxalp::InetAddr server_addr(12345);
    yxalp::Dispatcher dispatcher_;
    Socket sock(4);
    InetAddr client_addr(11223);
    std::string conn_name("wh");
   TcpConnectionPtr conn = std::make_shared<TcpConnection> (&dispatcher_, 
                                                         std::move(sock), client_addr, conn_name);  // hold 1
    std::map<std::string, TcpConnectionPtr> connections_;
    conn->set_connection_call_back(onConnection);
    // 更新 map
    connections_.insert(std::pair<std::string, TcpConnectionPtr> (conn_name, conn)); 
    conn->set_connection_established();

    return 0;
}
