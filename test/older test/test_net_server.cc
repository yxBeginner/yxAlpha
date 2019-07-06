#include <iostream>
#include <cstring>
#include "../net/inetaddr.h"
#include "../net/socket.h"

#define BUF_SIZE 100

int main() {
    using namespace yxalp;
    InetAddr serv_addr(12345);
    int raw_serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Socket serv_sock(raw_serv_sock);
    serv_sock.Bind(serv_addr);
    serv_sock.Listen();
    char buffer[BUF_SIZE] = {};
    while (true) {
        struct sockaddr_in con_addr;
        InetAddr con(con_addr);
        int connfd = serv_sock.Accept(con);
        Socket con_sock(connfd);
        std::cout << connfd << std::endl;
        ssize_t n = recv(connfd, buffer, BUF_SIZE, 0);
        std::cout << buffer << std::endl;
        send(connfd, buffer, n, 0);
    }
    
    return 0;
}