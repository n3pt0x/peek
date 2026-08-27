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

static inline int is_valid_port(int port)
{
    return 1 < port && port <= 65535;
}

static int parse_port_value(const char *str, Args *args)
{
    char *endptr;
    int port = strtoul(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        fprintf(stderr, "[Error] Port value is incorrect: %s\n", str);
        return -1;
    }

    if (is_valid_port(port) == 0) {
        fprintf(stderr, "[Error] Port value must be between 0 and 65535: %i\n", port);
        return -1;
    }

    args->port = port;
    return 0;
}

static int parse_port_range(const char *str, Args *args)
{
    char *copy = strdup(str);
    if (!copy) {
        fprintf(stderr, "[Error] Memory allocation failed\n");
        return -1;
    }

    char *min_str = strtok(copy, "-");
    char *max_str = strtok(NULL, "-");

    if (!min_str || !max_str) {
        fprintf(stderr, "[Error] Invalid port range '%s'\n", str);
        goto cleanup;
    }

    unsigned long min = strtoul(min_str, NULL, 10);
    unsigned long max = strtoul(max_str, NULL, 10);

    if (is_valid_port(min) == 0 || is_valid_port(max) == 0 || min > max) {
        fprintf(stderr, "[Error] Invalid port range '%s'\n", str);
        goto cleanup;
    }

    args->min_port = min;
    args->max_port = max;
    args->flags |= SCAN_RANGE;
    SAFE_FREE(copy);
    return 0;

cleanup:
    SAFE_FREE(copy);
    return -1;
}

static int parse_port(const char *str, Args *args)
{
    if (strcmp(str, "-") == 0) {
        args->min_port = 1;
        args->max_port = 65535;
        args->flags |= SCAN_RANGE;
        return 0;
    }

    if (strstr(str, "-") != 0)
        return parse_port_range(str, args);

    return parse_port_value(str, args);
}

int parse_args(int argc, char **argv, Args *args)
{
    // Default value
    memset(args, 0, sizeof(Args));
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
                fprintf(stderr, "[Error] Timeout value must be between 1 and 200\n");
                return -1;
            }
            args->timeout = (int)timeout;
            break;
        }
        }
    }

    /* Parse IP */
    // getopt move unused arguments at the end of argv array
    if (!parse_ip(argv[optind], args)) {
        fprintf(stderr, "IP format isn't valid: %s\n", argv[optind]);
        return -1;
    }

    return 0;
}

void debug_args(const Args *args)
{
    if (args->ip)
        printf("%s\n", args->ip);

    if (args->min_port && args->max_port) {
        printf("%i\n", args->min_port);
        printf("%i\n", args->max_port);
    } else {
        printf("%i\n", args->port);
    }

    if (args->timeout) {
        printf("%d\n", args->timeout);
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