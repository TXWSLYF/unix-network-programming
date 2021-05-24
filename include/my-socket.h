#ifndef MY_SOCKET
#define MY_SOCKET

#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int get_server_fd(int port)
{
    auto serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 解决 socket 关闭之后，端口占用问题
    // https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr/25193462
    int enable = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    /**
     * 将套接字和 IP、端口绑定
     */
    struct sockaddr_in serv_addr;             // 创建 socket 网络格式的地址
    memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都填充为 0
    serv_addr.sin_family = AF_INET;           // 设置协议簇
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);
    bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    return serv_sock;
}

#endif // MY_SOCKET