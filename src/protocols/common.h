#ifndef SCAN_UTILS_H
#define SCAN_UTILS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    PORT_UNKNOWN, // Error return after packet sending
    PORT_OPEN,
    PORT_CLOSED,
    PORT_FILTERED,
    PORT_UNREACHABLE
} PortState;

typedef struct StatusPort {
    uint16_t port;
    PortState status;
} StatusPort;

#define DROP_FD -1 // Assign pollfd.fd to -1

uint16_t checksum(const uint8_t *data, size_t data_len);
void generate_random_data(void *packet, size_t packet_len);

#endif /* SCAN_UTILS_H */