#ifndef UTILS_H
#define UTILS_H

#include "protocols/common.h"
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

static inline struct timeval ms_to_timeval(int ms) { return (struct timeval) { .tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000 } ; }
static inline bool is_valid_port(int port) { return port > 0 && port <= 65535; }
static inline bool is_valid_range_port(int min_port, int max_port) { return is_valid_port(min_port) && is_valid_port(max_port) && min_port < max_port; }
static inline bool is_valid_timeout(int timeout_ms) { return timeout_ms >= 1000 && timeout_ms <= 200000; }
static inline bool is_valid_ttl(int ttl) { return ttl > 1 && ttl <= 255; }
static inline bool is_valid_ip(const char *ip){ struct sockaddr_in sa; return inet_pton(AF_INET, ip, &(sa.sin_addr)); }

void print_port_status(uint16_t port, PortState port_state, uint32_t flags);

#endif /* UTILS_H */