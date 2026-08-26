#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

typedef struct Args {
    int s_type;
    char *ip;
    uint16_t port;
    uint16_t min_port;
    uint16_t max_port;
    int timeout;
    uint32_t flags;
} Args;

int parse_args(int argc, char **argv, Args *args);
void debug_args(const Args *args);
void free_args(Args *args);
void print_usage(const char *program_name);

#endif /* ARGS_H */