#include "socket.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int init_socket(int type)
{
    int sock;
    sock = socket(AF_INET, type, 0);

    if (sock < 0) {
        return -1;
    }

    struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    return sock;
}