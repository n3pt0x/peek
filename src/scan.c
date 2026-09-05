#include "scan.h"
#include "flags.h"
#include "scan/icmp.h"
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

static int handle_icmp(const Args *args)
{
    uint8_t packet[1024], response[1024];
    size_t packet_len;
    size_t payload_len = 56;
    EchoRequest req = {0};
    req.sequence = 1;
    strncpy(req.target, args->ip, sizeof(req.target) - 1);

    if (icmp_create_sock(&req) < 0) {
        fprintf(stderr, "[Error] Failed tro create ICMP socket: %s\n", strerror(errno));
        return -1;
    }

    if (args->ttl)
        if (icmp_set_ttl(&req, args->ttl) < 0)
            return -1;

    if (args->timeout)
        if (icmp_set_timeout(&req, args->timeout * 1000) < 0)
            return -1;

    if (icmp_build_echo(&req, packet, &packet_len, payload_len) < 0)
        return -1;

    if (icmp_send_packet(&req, packet, packet_len) < 0)
        return -1;

    if (icmp_recv_reply(&req, response, sizeof(response)) < 0)
        return -1;

    
    
    return 0;
}

int handle_scan(const Args *args)
{
    if (args->s_type == SOCK_STREAM) {
        return handle_tcp_scan(args);
    } else if (args->flags & SCAN_ICMP) {
        return handle_icmp(args);
    } else {
        // Comming Soon;
    }

    return -1;
}