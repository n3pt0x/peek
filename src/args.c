#include "args.h"
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
    int port = atol(str);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Port must be between 0 and 65535");
        return -1;
    }

    // Port range
    if (strstr(str, "-") != 0) {
        const char *port_range = strtok(str, "-");
        args->port_min = atol(port_range);

        port_range = strtok(NULL, "-");
        args->port_max = atol(port_range);
    } else {
        args->port = atol(str);
    }

    return 0;
}

static int set_socket_type(Args *args, int type)
{
    if (!type) {
        return -1;
    }

    if (type == SOCK_STREAM) {
        args->s_type = SOCK_STREAM;
        return 0;
    }

    if (type == SOCK_DGRAM) {
        args->s_type = SOCK_DGRAM;
        return 0;
    }

    return -1;
}

int set_timeout(Args *args, uint32_t timeout)
{
    if (!timeout) {
        return -1;
    }

    if (timeout <= 0) {
        return -1;
    }

    if (timeout > 200) {
        return -1;
    }

    args->timeout = timeout;
    return 0;
}

int parse_args(int argc, char **argv, Args *args)
{
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
            if (set_socket_type(args, SOCK_DGRAM) != 0)
                return -1;
            break;
        case 't':
            if (set_timeout(args, strtoul(optarg, NULL, 10)) != 0) {
                fprintf(stderr, "Timeout value must be between 1 and 65535");
                return -1;
            }
            break;
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
