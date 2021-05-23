#include "my-socket.h"
#include <iostream>
#include <pthread.h>

#define BUF_SIZE 100

using namespace std;

void *receive(void *client_fd)
{
    int _client_fd = *((int *)client_fd);
    cout << "进入子线程运行，客户端：" << _client_fd << endl;
    char buffer[BUF_SIZE] = {0};

    while (1)
    {
        int read_len = read(_client_fd, buffer, BUF_SIZE);

        // 客户端断开连接
        if (read_len == 0)
        {
            cout << "客户端断开连接：" << _client_fd << endl;
            close(_client_fd);

            cout << "退出子线程" << endl;
            int retval = 1;
            pthread_exit(&retval);
        }
        else
        {
            write(_client_fd, buffer, read_len);
        }
    }
}

int main()
{
    int server_fd = get_server_fd(8080);
    cout << "建立服务器：" << server_fd << endl;

    pthread_t id;
    while (1)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        cout << "接收客户端请求：" << client_fd << endl;

        /**
         * 创建子线程，处理来自客户端的请求
         * https://man7.org/linux/man-pages/man3/pthread_create.3.html
         */
        int ret = pthread_create(&id, NULL, receive, &client_fd);
        if (ret != 0)
        {
            cout << "创建子线程失败" << endl;
            exit(ret);
        }
        cout << "创建子线程成功：" << id << endl;
    }

    close(server_fd);

    return 0;
}