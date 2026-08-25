#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void parse_ip(const char *ip, Arguments *args);
void parse_port(char *str, Arguments *args);

void parse_arguments(char **arguments, Arguments *args)
{
    /* IP */
    if (arguments[1])
    {
        parse_ip(arguments[1], args);
    }

    for (int i = 0; arguments[i] != NULL; i++)
    {
        if (strcmp(arguments[i], "-p") == 0)
        {
            if (arguments[i + 1])
            {
                parse_port(arguments[i + 1], args);
            }
            else
            {
                printf("Arguments -p is empty");
                exit(EXIT_FAILURE);
            }
            continue;
        }

        if (strcmp(arguments[i], "-u") == 0)
        {
            args->s_type = SOCK_DGRAM;
            continue;
        }
    }
}

bool is_valid_ip(const char *ip)
{
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip, &(sa.sin_addr)))
    {
        return true;
    }
    return false;
}

void parse_ip(const char *ip, Arguments *args)
{
    if (is_valid_ip(ip))
    {
        args->ip = ip;
    }
}

void parse_port(char *str, Arguments *args)
{
    int port = atol(str);

    if (port == 0)
    {
        printf("Port must be between 0 and 65535");
    }

    // Port range
    if (strstr(str, "-") != 0)
    {
        const char *port_range = strtok(str, "-");
        args->port_min = atol(port_range);

        port_range = strtok(NULL, "-");
        args->port_max = atol(port_range);
    }
    else
    {
        args->port = atol(str);
    }
}

void debug_args(Arguments *args)
{
    if (args->ip)
    {
        printf("%s\n", args->ip);
    }
    if (args->port_min && args->port_max)
    {
        printf("%i\n", args->port_min);
        printf("%i\n", args->port_max);
    }
    else
    {
        printf("%i\n", args->port);
    }
    printf("%s\n", args->s_type == SOCK_STREAM ? "SOCK_STREAM" : "SOCK_DGRAM");
}
