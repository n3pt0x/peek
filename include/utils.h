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

static inline bool is_valid_port(int port) { return port > 0 && port <= 65535; }

bool is_valid_ip(const char *ip);

#endif /* UTILS_H */