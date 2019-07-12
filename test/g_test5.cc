// 一个 echo server
#include "../net/tcpserver.h"
#include "../reactor/dispatcher.h"
#include "../net/inetaddr.h"
#include <stdio.h>

std::string message;

void OnConnection(const yxalp::TcpConnectionPtr& conn) {
    if (conn->is_connected()) {
        printf("onConnection(): new connection [%s],\n",
            conn->get_name().c_str());
        conn->Send(message);
    } else {
        printf("onConnection(): connection [%s] is down\n",
            conn->get_name().c_str());
    }
}

void OnWriteComplete(const yxalp::TcpConnectionPtr& conn) {
    conn->Send(message);
}

void OnMessage(const yxalp::TcpConnectionPtr& conn, yxalp::Buffer *buffer) {
    size_t data_size = buffer->payload_size();
    std::string message = buffer->GetString();
    printf("onMessage(): received %zd bytes from connection [%s], content: %s\n",
        data_size, conn->get_name().c_str(), message.c_str());
    buffer->move_read_index_all();
}

int main() {
    yxalp::InetAddr server_addr(12345);
    yxalp::Dispatcher dispatcher;
    
    std::string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;
    for (size_t i = 0; i < 127-33; ++i) {
        message += line.substr(i, 72) + '\n';
    }

    yxalp::TcpServer server(&dispatcher, server_addr);
    server.set_connection_call_back(OnConnection);
    server.set_message_call_back(OnMessage);
    server.set_write_complete_callback(OnWriteComplete);
    server.Start();

    dispatcher.Loop();

    return 0;
}
