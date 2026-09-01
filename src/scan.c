#include "scan.h"
#include "flags.h"
#include "scan/tcp.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int handle_tcp_scan(const Args *args)
{
    if (args->flags & SCAN_RANGE) {
        StatusPort *open_port = malloc(65535 * sizeof(StatusPort));
        memset(open_port, 0, sizeof(*open_port));
        int count = 0;

        if (scan_range_port(args, open_port, &count) < 0) {
            return -1;
        }

        for (int i = 0; i < count; i++) {
            int port = open_port[i].port;
            int status = open_port[i].status;

            if (port > 0) {
                if (status == 0)
                    printf("Port %d is open\n", port);
                // else if (status == 1)
                //     printf("Port %d is closed\n", args->port);
                else if (status == 1)
                    printf("Port %d is filtered\n", port);
            }
        }
        SAFE_FREE(open_port);
        return 0;
    }

    if (args->port) {
        int state = scan_single_tcp_port(args, args->port);
        if (state < 0) {
            if (errno == ECONNREFUSED)
                printf("Port %d is closed\n", args->port);
            else if (errno == ETIMEDOUT)
                printf("Port %d is filtered\n", args->port);
            else {
                fprintf(stderr, "Port %d error: %s", args->port,
                        strerror(errno));
                return -1;
            }

            return 0;
        }

        printf("Port %d is %s\n", args->port, (state == 0) ? "open" : "closed");
        return 0;
    }

    return -1;
}

int handle_scan(const Args *args)
{
    if (args->s_type == SOCK_STREAM) {
        return handle_tcp_scan(args);
    } else {
        // Comming Soon;
    }

    return -1;
}