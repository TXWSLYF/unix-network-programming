#include <sys/epoll.h>
#include "my-socket.h"
#include <iostream>

#define MAX_EVENTS 10
#define BUF_SIZE 100

int main()
{
    char buffer[BUF_SIZE] = {0};
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    int server_fd = get_server_fd(8002);

    // 创建 epoll 的文件描述符
    int epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        std::cerr << "[E] epoll_create1 failed\n";
        return 1;
    }

    /**
     * 将服务器 fd 添加到 epoll 监听列表
     * event.data 携带自定义数据
     * event.events 指定感兴趣的事件
     * EPOLLIN: The associated file is available for read(2) operations
     * EPOLLET:Requests edge-triggered(边缘触发) notification for the associated file descriptor
     * TODO: 边缘触发和水平触发的区别和适用场景
     */
    event.data.fd = socketfd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event) != 0)
    {
        exit(errno);
    }

    while (1)
    {
        /**
         * epoll_wait 是一个阻塞的系统调用
         * 返回值表示的是准备好的文件描述符的数量，不会大于 MAX_EVENTS
         * 第四个参数指定了最大阻塞时长，-1 表示一直阻塞
         */
        int event_count = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < event_count; ++i)
        {
            if (events[i].events & EPOLLERR ||
                events[i].events & EPOLLHUP ||
                !(events[i].events & EPOLLIN)) // error
            {
                std::cout << "出错，关闭文件描述符:" << events[i].data.fd << std::endl;
                close(events[i].data.fd);
            }
            else if (events[i].data.fd == server_fd) // 新建连接
            {
                int client_fd = accept(server_fd, NULL, NULL);
                event.data.fd = client_fd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, event);
                std::cout << "新建连接:" << client_fd << std::endl;
            }
            else
            {
                int fd = events[i].data.fd;
                int read_len = read(fd, buffer, BUF_SIZE);

                if (read_len == -1)
                {
                    std::cout << "读取数据错误:" << fd << std::endl;
                    exit(-1);
                }
                else if (read_len == 0)
                {
                    std::cout << "关闭客户端连接:" << fd << std::endl;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                else
                {
                    std::cout << "返回数据:" << fd << std::endl;
                    write(fd, buffer, read_len);
                }
            }
        }
    }

    close(server_fd);

    return 0;
}
