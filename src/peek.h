#ifndef PEEK_H
#define PEEK_H

#include <stdint.h>

/* Config */
#define DEFAULT_TIMEMOUT_MS 2000 // MS

/* Flags */
#define SCAN_VERBOSE (1U << 0)
#define SCAN_NETWORK (1U << 1)
#define SCAN_RANGE (1U << 2)
#define SCAN_TCP (1U << 3)
#define SCAN_UDP (1U << 4)
#define SCAN_ICMP (1U << 5)

/* Struct */
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

#endif /* PEEK_H */