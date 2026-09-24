#ifndef SCAN_UTILS_H
#define SCAN_UTILS_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    PORT_UNKNOWN,
    PORT_OPEN,
    PORT_CLOSED,
    PORT_FILTERED
} PortState;

typedef struct StatusPort {
    uint16_t port;
    PortState status;
} StatusPort;

uint16_t checksum(const uint8_t *data, size_t data_len);
void generate_random_data(void *packet, size_t packet_len);

#endif /* SCAN_UTILS_H */