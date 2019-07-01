// yx
#include "tcpserver.h"

#include "socket.h"
#include "inetaddr.h"

namespace yxalp {

void TcpServer::Start() {
    
}

void TcpServer::NewConnection(Socket sock, const InetAddr &client_addr) {
    // log accept new connection
    TcpConnectionPtr conn = std::make_shared<TcpConnection> (dispatcher_, sock, client_addr);
    conn->set_connection_call_back(connection_call_back_);
    conn->set_message_call_back(message_call_back_);
    conn->set_connection_established();
}


}  // namespace yxalp 
