#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100

using namespace std;

int main()
{
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[BUF_SIZE] = {0};
    string ip = "127.0.0.1";
    int port;

    cout << "请输入 port:" << endl;
    cin >> port;

    // 向服务器（特定 IP + port）发送请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    serv_addr.sin_port = htons(port);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1)
    {
        string message;
        cout << "请输入要发送的消息:" << endl;
        cin >> message;
        write(sock, message.c_str(), sizeof(message.c_str()));

        while (1)
        {
            int read_len = read(sock, buffer, BUF_SIZE);
            std::cout << "读取服务端数据：" << read_len << std::endl;

            if (read_len < 0)
            {
                exit(read_len);
            }
            // 服务器主动断开连接时会触发
            if (read_len == 0)
            {
                std::cout << "读取完毕！" << std::endl;
                break;
            }
        }
    }

    //关闭套接字
    close(sock);

    return 0;
}