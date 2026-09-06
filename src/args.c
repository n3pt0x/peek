#include "args.h"
#include "utils/utils.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"udp", no_argument, 0, 'u'},
    {"icmp", no_argument, 0, 'i'},
    {"ttl", required_argument, 0, 'l'},
    {"timeout", required_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static bool parse_ip(const char *ip, Args *args)
{
    if (is_valid_ip(ip)) {
        args->target = strdup(ip);
        return true;
    }

    return false;
}

static int parse_port_value(const char *str, Args *args)
{
    char *endptr;
    int port = strtoul(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        fprintf(stderr, "[Error] Port value is incorrect: %s\n", str);
        return -1;
    }

    if (!is_valid_port(port)) {
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

    if (!is_valid_range_port(min, max)) {
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
    args->flags |= SCAN_NETWORK;

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:u::i::l:t:vh", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'p':
            if (parse_port(optarg, args) != 0)
                return -1;
            break;
        case 'u':
            args->flags |= SCAN_NETWORK;
            args->s_type = SOCK_DGRAM;
            break;
        case 'i':
            /* Disable default scan (TCP) */
            args->s_type = 0;
            args->flags =~ SCAN_NETWORK;
            
            args->flags |= SCAN_ICMP;
            break;
        case 'l': {
            int ttl = atoi(optarg);
            if(!is_valid_ttl(ttl)) {
                fprintf(stderr, "[Error] TTL value must be between 1 and 255\n");
                return -1;
            }
            args->ttl = ttl;
            break;
        }
        case 't': {
            unsigned long timeout = strtoul(optarg, NULL, 10) * 1000; // Convert to MS
            if (!is_valid_timeout(timeout)) {
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
    if (args->target)
        printf("%s\n", args->target);

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
    if (args->target) {
        free(args->target);
        args->target = NULL;
    }
}