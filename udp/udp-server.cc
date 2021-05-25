#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF 10000

int main()
{
    char buffer[MAX_BUF] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_struct_length = sizeof(client_addr);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // create UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_fd < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    printf("Listening for incoming messages...\n\n");

    while (1)
    {
        int read_len = recvfrom(server_fd, buffer, MAX_BUF, 0, (struct sockaddr *)&client_addr, &client_struct_length);
        if (read_len < 0)
        {
            printf("Couldn't receive\n");
            return -1;
        }

        printf("Received message from IP: %s and port: %i\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        sendto(server_fd, buffer, read_len, 0, (struct sockaddr *)&client_addr, client_struct_length);
    }

    // Close the socket:
    close(server_fd);

    return 0;
}