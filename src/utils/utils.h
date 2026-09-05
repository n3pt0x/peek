#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

#define SAFE_FREE(ptr)                                                         \
    do {                                                                       \
        if (ptr) {                                                             \
            free(ptr);                                                         \
            (ptr) = NULL;                                                      \
        }                                                                      \
    } while (0)

static inline bool is_valid_port(int port) { return port > 0 && port <= 65535; }
static inline bool is_valid_range_port(int min_port, int max_port) { return is_valid_port(min_port) && is_valid_port(max_port) && min_port < max_port; }
static inline bool is_valid_timeout(int timeout_ms) { return timeout_ms >= 1000 && timeout_ms <= 200000; }
static inline bool is_valid_ttl(int ttl) { return ttl > 1 && ttl <= 255; }

bool is_valid_ip(const char *ip);

#endif /* UTILS_H */