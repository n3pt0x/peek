#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef struct Args {
    int s_type;
    char *target;
    uint16_t port;
    uint16_t min_port;
    uint16_t max_port;
    int timeout;
    int ttl;
    uint32_t flags;
} Args;

#endif /* COMMON_H */