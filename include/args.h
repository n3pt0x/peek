#ifndef ARGS_H
#define ARGS_H

typedef struct Arguments
{
    int s_type; 
    const char *ip;
    int port;
    int port_min;
    int port_max;
} Arguments;

void parse_arguments(char **arguments, Arguments *args);

#endif /* ARGS_H */