#include "socket.h"
#include "utils/utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int socket_create(int domain, int type, int protocol)
{
    int sock;
    sock = socket(domain, type, protocol);

    if (sock < 0) {
        return -1;
    }

    return sock;
}

int socket_set_timeout(int *sock, int timeout_ms)
{
    if (*sock < 0) {
        return -1;
    }

    if (!is_valid_timeout(timeout_ms)) {
        fprintf(stderr, "[Error] Timeout must be between 1 and 200 seconds\n");
        return -1;
    }

    struct timeval tv = ms_to_timeval(timeout_ms);
    if (setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "[Error] Timeout SO_RCVTIMEO failed: %s\n", strerror(errno));
        return -1;
    }

    if (setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "[Error] Timeout SO_SNDTIMEO failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int socket_set_ttl(int *sock, int ttl)
{
    if (*sock < 0) {
        return -1;
    }

    if (!is_valid_ttl(ttl)) {
        fprintf(stderr, "[Error] TTL must be between 1 and 255\n");
        return -1;
    }

    return setsockopt(*sock, IPPROTO_ICMP, IP_TTL, (const void *)&ttl,
                      sizeof(ttl));
}