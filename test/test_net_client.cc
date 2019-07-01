#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100

int main(){
    struct sockaddr_in serv_addr;  // _in 代表Internet
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(12345);  //端口
    char buffer[BUF_SIZE] = {"I do not know."};
    char buf_return[BUF_SIZE];
    // int sock = socket(AF_INET, SOCK_STREAM, 0);  // 由于connect被断开，因而此socket...咋了
    while (1) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        // 注意客户端指定的是server的地址，不是自己的(ip/端口)
        int check = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        printf("connect state: %d\n", check);  //'check' --> Return 0 on success, -1 for errors
        // printf("Input String:");
        // scanf("%s", buffer);
        // buffer[strlen(buffer)] ='\0';
        printf("Sending : %s\n with len %d", buffer, strlen(buffer));
        send(sock, buffer, strlen(buffer), 0);
        sleep(3);
        recv(sock, buf_return, BUF_SIZE, 0);
        // read(sock, buffer, sizeof(buffer)-1);
        printf("Message form server: %s\n", buf_return);
        //关闭套接字
        close(sock);
    }
    return 0;
}

