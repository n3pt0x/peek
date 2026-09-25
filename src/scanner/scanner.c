#include "scanner.h"
#include "net/socket.h"
#include "peek.h"
#include "protocols/icmp.h"
#include "protocols/tcp.h"
#include "protocols/udp.h"
#include "utils.h"
#include "utils/utils.h"
#include <asm-generic/errno-base.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static int set_timeout(int *socket, int timeout)
{
    if (timeout) {
        if (socket_set_timeout(socket, timeout) < 0)
            return -1;
    } else {
        if (socket_set_timeout(socket, DEFAULT_TIMEMOUT_MS) < 0)
            return -1;
    }

    return 0;
}

static int handle_tcp_scan(const Args *args)
{
    if (args->flags & SCAN_RANGE) {
        StatusPort *open_port =
            malloc((args->max_port - args->min_port) * sizeof(StatusPort));
        memset(open_port, 0, sizeof(*open_port));
        size_t port_scanned = 0;

        if (tcp_connect_scan_range(args, open_port, &port_scanned) < 0) {
            fprintf(stderr, "[Error] Scan range port has failed: %s\n",
                    strerror(errno));
            return -1;
        }

        for (size_t i = 0; i < port_scanned; i++) {
            int port = open_port[i].port;
            int status = open_port[i].status;

            print_port_status(port, status, args->flags);
        }
        SAFE_FREE(open_port);
        return 0;
    }

    if (args->port) {
        PortState port_state;
        int state = tcp_connect_scan(args, args->port, &port_state);

        if (state < 0) {
            if (port_state == PORT_UNKNOWN)
                return state;
        }

        print_port_status(args->port, state, args->flags);
        return 0;
    }

    return -1;
}

static int handle_udp_scan(const Args *args)
{
    int sock = udp_create_socket();
    if (sock < 0) {
        fprintf(stderr, "[Error] socket: %s\n", strerror(errno));
        return -1;
    }

    if (set_timeout(&sock, args->timeout) < 0) {
        close(sock);
        return -1;
    }

    int state = udp_connect_scan(sock, args->target, args->port);

    close(sock);

    if (state == 0)
        printf("Port %d is open\n", args->port);
    else if (state == EAGAIN)
        printf("Port %d is filtered\n", args->port);
    else if (state == ECONNREFUSED)
        printf("Port %d is closed\n", args->port);
    else
        printf("Port %d is closed\n", args->port);

    return 0;
}

static int handle_icmp_scan(const Args *args)
{
    uint8_t packet[1024], response[1024];
    size_t packet_len;
    size_t payload_len = 56;
    EchoRequest req = {0};
    req.sequence = 1;
    strncpy(req.target, args->target, sizeof(req.target) - 1);

    /* Socket Config*/
    if ((req.sock = socket_create(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0) {
        fprintf(stderr, "[Error] Failed tro create ICMP socket: %s\n",
                strerror(errno));
        return -1;
    }

    if (args->timeout) {
        if (socket_set_timeout(&req.sock, args->timeout) < 0)
            return -1;
    } else {
        if (socket_set_timeout(&req.sock, DEFAULT_TIMEMOUT_MS) < 0)
            return -1;
    }

    if (args->ttl)
        if (socket_set_ttl(&req.sock, args->ttl) < 0)
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
    if (args->flags & SCAN_TCP)
        return handle_tcp_scan(args);
    else if (args->flags & SCAN_UDP)
        return handle_udp_scan(args);
    else if (args->flags & SCAN_ICMP)
        return handle_icmp_scan(args);

    return -1;
}