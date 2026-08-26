#include "args.h"
#include "flags.h"
#include "utils.h"
#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static const struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"udp", no_argument, 0, 'u'},
    {"timeout", required_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static bool parse_ip(const char *ip, Args *args)
{
    if (is_valid_ip(ip)) {
        args->ip = strdup(ip);
        return true;
    }

    return false;
}

static int parse_port(char *str, Args *args)
{
    int port = strtoul(str, NULL, 10);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "[Error] Port value must be between 0 and 65535: %i\n", port);
        return -1;
    }

    args->port = port;
    return 0;
}

int parse_args(int argc, char **argv, Args *args)
{
    // Default value
    args->s_type = SOCK_STREAM;

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:u::t:vh", long_options,
                              &option_index)) != -1) {
        switch (opt) {
        case 'p':
            if (parse_port(optarg, args) != 0)
                return -1;
            break;
        case 'u':
            args->flags |= SCAN_UDP;
            args->s_type = SOCK_DGRAM;
            break;
        case 't': {
            unsigned long timeout = strtoul(optarg, NULL, 10);
            
            if (timeout < 1 || timeout > 200) {
                fprintf(stderr, "[Error] Timeout value must be between 1 and 200: %lu\n", timeout);
                return -1;
            }
            break;
        }
        }
    }

    /* Parse IP */
    // getopt move unused arguments at the end of argv array
    if (!parse_ip(argv[optind], args)) {
        fprintf(stderr, "IP format isn't valid: %s", argv[optind]);
        return -1;
    }

    return 0;
}

void debug_args(const Args *args)
{
    if (args->ip)
        printf("%s\n", args->ip);

    if (args->port_min && args->port_max) {
        printf("%i\n", args->port_min);
        printf("%i\n", args->port_max);
    } else {
        printf("%i\n", args->port);
    }
    printf("%s\n", args->s_type == SOCK_STREAM ? "SOCK_STREAM" : "SOCK_DGRAM");
}

void free_args(Args *args)
{
    if (args->ip) {
        free(args->ip);
        args->ip = NULL;
    }
}