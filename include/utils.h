#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#define SAFE_FREE(ptr)                                                         \
    do {                                                                       \
        if (ptr) {                                                             \
            free(ptr);                                                         \
            (ptr) = NULL;                                                      \
        }                                                                      \
    } while (0)

bool is_valid_ip(const char *ip);

#endif /* UTILS_H */