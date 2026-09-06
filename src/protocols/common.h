#ifndef SCAN_UTILS_H
#define SCAN_UTILS_H

#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <stddef.h>

static inline struct timeval ms_to_timeval(int ms)
{
    return (struct timeval){.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
}

uint16_t checksum(const uint8_t *data, size_t data_len);

#endif /* SCAN_UTILS_H */