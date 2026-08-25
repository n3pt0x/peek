#pragma once

#include <sys/socket.h>
#include <stdbool.h>

typedef struct Arguments
{
    int s_type; 
    const char *ip;
    int port;
    int port_min;
    int port_max;
} Arguments;

void parse_arguments(char **arguments, Arguments *args);
bool is_valid_ip(const char *ip);
void parse_ip(const char *ip, Arguments *args);
void parse_port(char *str, Arguments *args);
