#ifndef TCP_H
#define TCP_H

#include "peek.h"
#include "common.h"
#include <stddef.h>
#include <stdint.h>

int tcp_connect_scan(const Args *args, uint16_t port, PortState *port_state);
int tcp_connect_scan_range(const Args *args, StatusPort *open_port, size_t *port_scanned);

#endif /* TCP_H */