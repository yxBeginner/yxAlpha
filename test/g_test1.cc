// 更新了 buffer 相关, 该测试失效
#include "../net/tcpserver.h"
#include "../reactor/dispatcher.h"
#include "../net/inetaddr.h"
#include <stdio.h>

void onConnection(const yxalp::TcpConnectionPtr& conn) {
    // printf("onConnection(): shared ptr holder : %ld \n", conn.use_count());
    yxalp::TcpConnectionPtr local_ptr = conn;
    if (conn->is_connected()) {
        printf("onConnection(): new connection [%s],\n",
            conn->get_name().c_str());
            // conn-> ->peerAddress().toHostPort().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n",
            conn->get_name().c_str());
    }
}

void onMessage(const yxalp::TcpConnectionPtr& conn,
                                    const char* data, ssize_t len) {
  printf("onMessage(): received %zd bytes from connection [%s], content: %s\n",
         len, conn->get_name().c_str(), data);
}

int main() {
    yxalp::InetAddr server_addr(12345);
    yxalp::Dispatcher dispatcher;
    
    yxalp::TcpServer server(&dispatcher, server_addr);
    server.set_connection_call_back(onConnection);
    server.set_message_call_back(onMessage);
    server.Start();

    dispatcher.Loop();

    return 0;
}
