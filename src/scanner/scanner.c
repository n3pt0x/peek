#include "scanner.h"
#include "common/flags.h"
#include "common/config.h"
#include "protocols/icmp.h"
#include "protocols/tcp.h"
#include "utils.h"
#include "utils/utils.h"

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
                fprintf(stderr, "Port %d error: %s\n", args->port,
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
    strncpy(req.target, args->target, sizeof(req.target) - 1);

    /* Socket Config*/
    if (icmp_create_sock(&req) < 0) {
        fprintf(stderr, "[Error] Failed tro create ICMP socket: %s\n",
                strerror(errno));
        return -1;
    }

    if (args->timeout) {
        if (icmp_set_timeout(&req, args->timeout) < 0)
            return -1;
    } else {
        if (icmp_set_timeout(&req, DEFAULT_TIMEMOUT_MS) < 0)
            return -1;
    }

    if (args->ttl)
        if (icmp_set_ttl(&req, args->ttl) < 0)
            return -1;

    /* Build Packet */
    if (icmp_build_echo(&req, packet, &packet_len, payload_len) < 0)
        return -1;

    /* Send Packet */
    if (icmp_send_packet(&req, packet, packet_len) < 0)
        return -1;

    /* Handle response */
    int state = icmp_recv_reply(&req, response, sizeof(response));
    if (state < 0) {
        return -1;
    } else {
        if (state == 0) {
            printf("Host %s is up\n", req.target);
        } else if (state == 1) {
            printf("Host %s is down (unreachable)\n", req.target);
        }
    }

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