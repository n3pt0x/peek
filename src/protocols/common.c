#include "common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint16_t checksum(const uint8_t *data, size_t data_len)
{
    uint32_t sum = 0;
    uint16_t word;

    for (size_t i = 0; i + 1 < data_len; i += 2) {
        word = (data[i] << 8) | data[i + 1];
        sum += word;

        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    if (data_len & 1) {
        word = (data[data_len - 1] << 8) | 0;
        sum += word;

        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    return (uint16_t)~sum;
}

void generate_random_data(void *packet, size_t packet_len)
{
    uint8_t *data = packet;
    for (size_t i = 0; i < packet_len; i++) {
        data[i] = rand() % 256;
    }
}