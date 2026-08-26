#ifndef ARGS_H
#define ARGS_H

typedef struct Args {
    int s_type;
    const char *ip;
    int port;
    int port_min;
    int port_max;
} Args;

int parse_arguments(char **arguments, Args *args);
void debug_args(const Args *args);

#endif /* ARGS_H */