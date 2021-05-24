#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main()
{
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    string ip;
    int port;

    cout << "请输入 ip:" << endl;
    cin >> ip;
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

        // 读取服务器返回的数据
        char buffer[400];
        read(sock, buffer, 400);
        printf("Message form server: %s\n", buffer);
    }

    //关闭套接字
    close(sock);

    return 0;
}