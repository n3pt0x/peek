#ifndef TCP_H
#define TCP_H

#include "peek.h"
#include <stdint.h>

typedef struct StatusPort {
    uint16_t port;
    uint8_t status;
} StatusPort;

int scan_single_tcp_port(const Args *args, uint16_t port);
int scan_range_port(const Args *args, StatusPort *open_port, int *count);

#endif /* TCP_H */