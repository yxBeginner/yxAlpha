// 一个 echo server
#include "../net/tcpserver.h"
#include "../reactor/dispatcher.h"
#include "../net/inetaddr.h"
#include "../utility/buffer.h"
#include <stdio.h>
#include <unistd.h>

std::string message1;
std::string message2;

void onConnection(const yxalp::TcpConnectionPtr& conn) {
    if (conn->is_connected()) {
        printf("onConnection(): new connection [%s],\n",
            conn->get_name().c_str());
        if (!message1.empty()) {
            conn->Send(message1);
        }
        if (!message2.empty()) {
            conn->Send(message2);
        }
        conn->ShutDown();
    } else {
        printf("onConnection(): connection [%s] is down\n",
            conn->get_name().c_str());
    }
}

void onMessage(const yxalp::TcpConnectionPtr& conn, yxalp::Buffer *buffer) {
    size_t data_size = buffer->payload_size();
    std::string message = buffer->GetString();
    printf("onMessage(): received %zd bytes from connection [%s], content: %s\n",
        data_size, conn->get_name().c_str(), message.c_str());
    // conn->Send(message);
}

int main(int argc, char *argv[]) {
    printf("main(): pid = %d\n", getpid());
    int len1 = 100;
    int len2 = 200;
    if (argc > 2) {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }
    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    yxalp::InetAddr server_addr(12345);
    yxalp::Dispatcher dispatcher;
    
    yxalp::TcpServer server(&dispatcher, server_addr);
    server.set_connection_call_back(onConnection);
    server.set_message_call_back(onMessage);

    server.SetThreadNum(4);
    server.Start();

    dispatcher.Loop();

    return 0;
}
