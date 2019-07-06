#include "../net/tcpserver.h"
#include "../reactor/dispatcher.h"
#include "../net/inetaddr.h"
#include <stdio.h>

void onConnection(const yxalp::TcpConnectionPtr& conn) {
    if (conn->is_connected()) {
        printf("onConnection(): new connection [%s] from\n",
            conn->name().c_str());
            // conn-> ->peerAddress().toHostPort().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n",
            conn->name().c_str());
    }
}

void onMessage(const yxalp::TcpConnectionPtr& conn,
                                    const char* data, ssize_t len) {
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         len, conn->name().c_str());
}

int main() {
//   printf("main(): pid = %d\n", getpid());
    yxalp::InetAddr server_addr(12345);
    yxalp::Dispatcher dispatcher;
    
    yxalp::TcpServer server(&dispatcher, server_addr);
    server.set_connection_call_back(onConnection);
    server.set_message_call_back(onMessage);
    server.Start();

    dispatcher.Loop();

    return 0;
}
