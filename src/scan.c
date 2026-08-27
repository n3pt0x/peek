#include "scan.h"
#include "flags.h"
#include "socket.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int scan_single_tcp_port(const Args *args, uint16_t port)
{
    int sock, conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (!args->s_type || args->s_type != SOCK_STREAM) {
        fprintf(stderr, "[Error] TCP socket type is not valid\n");
        return -1;
    }

    sock = init_socket(args->s_type);

    if (sock < 0) {
        fprintf(stderr, "[Error] Socket creation failed %s\n", strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, args->ip, &(addr.sin_addr)) <= 0) {
        fprintf(stderr, "[Error] IP adress is invalid: %s\n", args->ip);
        return -1;
    }

    conn = connect(sock, (const struct sockaddr *)&addr, addr_len);

    close(sock);
    return conn;
}

static int scan_range_port(const Args *args, int *open_port, int *count)
{
    if (!is_valid_port(args->min_port) || !is_valid_port(args->max_port)) {
        return -1;
    }

    for (int i = 0, port = args->min_port; port < args->max_port; port++) {
        if (scan_single_tcp_port(args, port) < 0) {
            return -1;
        }

        open_port[i] = port;
        (*count)++;
    }

    return 0;
}

int handle_scan(const Args *args)
{
    if (args->flags & SCAN_RANGE) {
        int *open_port = (int *)malloc(65535);
        memset(open_port, 0, sizeof(*open_port));
        int count = 0;

        int state = scan_range_port(args, open_port, &count);
        if (state < 0) {
            return state;
        }

        for (int i = 0; i < count; i++) {
            int port = open_port[i];
            if (port != 0) {
                printf("Port %d is open", port);
            }
        }
        return state;
    }

    if (args->port) {
        int state = scan_single_tcp_port(args, args->port);
        if (state < 0) {
            if (errno == ECONNREFUSED)
                printf("Port %d is closed\n", args->port);
            else if (errno == ETIMEDOUT)
                printf("Port %d is filtered\n", args->port);
            else {
                fprintf(stderr, "Port %d error: %s", args->port, strerror(errno));
                return -1;
            }

            return 0;
        }

        printf("Port %d is %s\n", args->port, (state == 0) ? "open" : "closed");
        return 0;
    }

    return -1;
}