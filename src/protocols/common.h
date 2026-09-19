#ifndef SCAN_UTILS_H
#define SCAN_UTILS_H

#include <stdint.h>
#include <stddef.h>

uint16_t checksum(const uint8_t *data, size_t data_len);
void generate_random_data(void *packet, size_t packet_len);

#endif /* SCAN_UTILS_H */