#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

#define SAFE_FREE(ptr)                                                         \
    do {                                                                       \
        if (ptr) {                                                             \
            free(ptr);                                                         \
            (ptr) = NULL;                                                      \
        }                                                                      \
    } while (0)

bool is_valid_ip(const char *ip)
{
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip, &(sa.sin_addr)))
        return true;
    
    return false;
}

static inline bool is_valid_port(int port)
{
    if (1 < port && port <= 65535)
        return true;

    return false;
}

#endif /* UTILS_H */