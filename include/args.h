#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

typedef struct Args {
    int s_type;
    char *ip;
    int port;
    int port_min;
    int port_max;
    uint32_t timeout;
} Args;

int parse_args(int argc, char **argv, Args *args);
void debug_args(const Args *args);
void free_args(Args *args);
void print_usage(const char *program_name);

#endif /* ARGS_H */