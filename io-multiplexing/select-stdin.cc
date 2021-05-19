#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

int main()
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, nullptr, nullptr, &tv);

    if (retval == -1)
    {
        perror("select()");
    }
    else if (retval)
    {
        printf("Data is available now.\n");
    }
    else
    {
        printf("No data within five seconds.\n");
    }

    exit(EXIT_SUCCESS);
}