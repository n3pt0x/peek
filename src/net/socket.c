#include "socket.h"
#include <sys/socket.h>

int init_socket(const Args *args)
{
    int sock;
    sock = socket(AF_INET, args->s_type, 0);

    if (sock < 0) {
        return -1;
    }

    struct timeval tv = {.tv_sec = args->timeout, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    return sock;
}