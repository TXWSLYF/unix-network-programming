#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int connfd;
    socklen_t client_len;
    struct sockaddr_in client_address;
    fd_set readfds, testfds;

    // 创建套接字
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // 解决 socket 关闭之后，端口占用问题
    // https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr/25193462
    int enable = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    /**
     * 将套接字和 IP、端口绑定
     */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8080);
    bind(listenfd, (struct sockaddr *)&server_address, sizeof(server_address));

    // 创建套接字队列
    listen(listenfd, 5);

    FD_ZERO(&readfds);
    FD_SET(listenfd, &readfds);

    // 等待客户请求
    while (1)
    {
        int fd;
        int nread;
        char buf[100];

        // 每次运行 select 之前需要拷贝一份需要监听的文件描述符，因为 select 调用会修改入参
        testfds = readfds;
        printf("server waiting\n");
        /**
         * int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
         * nfds，限制了 select 能同时处理的 tcp 连接数，最大值一般为 1024，这是定义在系统的宏定义中的，因为取值太大的话，select 的性能会明显下降
         * readfds，监听可读事件的 fds
         * writefds，监听可写事件的 fds
         * exceptfds，监听异常事件的 fds
         * timeout，指定 select 阻塞的最长时长，如果为空指针的话，阻塞时长没有限制
         * return，-1 表示异常，0 表示到了到期时间，大于 1 表示准备好的 fd 的数量
         */
        int result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        if (result < 1)
        {
            perror("select error");
            exit(1);
        }

        // select 的性能瓶颈之一，就是每次都需要遍历所有的文件描述符，确定哪个描述符可读或者可写
        for (fd = 0; fd < FD_SETSIZE; fd++)
        {
            /**
             * FD_ISSET 用来检测某个文件描述符是否在 fd_set 当中，此时 testfds 当中肯定是可读的文件描述符
             */
            if (FD_ISSET(fd, &testfds))
            {
                // 如果是一个新的客户连接请求
                if (fd == listenfd)
                {
                    client_len = sizeof(client_address);
                    connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_len);
                    // 将对应的客户端 socket_fd 添加到监听队列中
                    FD_SET(connfd, &readfds);
                    printf("adding client on fd %d\n", connfd);
                }
                // 如果是旧的客户活动
                else
                {
                    ioctl(fd, FIONREAD, &nread);
                    // 如果客户断开连接
                    if (nread == 0)
                    {
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing client on fd %d\n", fd);
                    }
                    // 客户请求数据到达
                    else
                    {
                        // TODO:研究如何完整读取客户端发来的信息
                        read(fd, buf, 100);
                        printf("serving client on fd %d\n", fd);
                        write(fd, buf, 100);
                        memset(buf, 0, sizeof(buf));
                    }
                }
            }
        }
    }
}