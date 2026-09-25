#include "tcp.h"
#include "common.h"
#include "net/socket.h"
#include "utils/utils.h"
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

int tcp_connect_scan(const Args *args, uint16_t port, PortState *port_state)
{
    int sock, conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    PortState state = PORT_UNKNOWN;

    sock = socket_create(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        fprintf(stderr, "[Error] Socket creation failed %s\n", strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, args->target, &(addr.sin_addr)) <= 0) {
        fprintf(stderr, "[Error] IP adress is invalid: %s\n", args->target);
        return -1;
    }

    conn = connect(sock, (const struct sockaddr *)&addr, addr_len);

    close(sock);

    if (conn == 0) {
        state = PORT_OPEN;
    }

    switch (errno) {
    case ETIMEDOUT:
        state = PORT_FILTERED;
        break;
    case ECONNREFUSED:
        state = PORT_CLOSED;
        break;
    }

    *port_state = state;
    return conn;
}

int tcp_connect_scan_range(const Args *args, StatusPort *open_port,
                           size_t *port_scanned)
{
    if (!is_valid_range_port(args->min_port, args->max_port)) {
        return -1;
    }

    int ret = 0;

    uint16_t current_port = args->min_port;

    size_t nb_ports = (size_t)(args->max_port - args->min_port + 1);
    struct pollfd fds[nb_ports];
    size_t fds_nb = 0;

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, args->target, &addr.sin_addr) < 0) {
        perror("inet_pton");
        return -1;
    }

    for (size_t i = 0; i < nb_ports; i++) {
        int sock = socket_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sock < 0) {
            perror("socket");
            return -1;
        }

        if (set_nonblocking(sock) < 0) {
            close(sock);
            ret = -1;
            goto cleanup;
        }

        fds[i].fd = sock;
        fds[i].events = POLLOUT;
        fds[i].revents = 0;
        fds_nb++;

        open_port[i].port = current_port;
        addr.sin_port = htons(current_port++);
        int result = connect(fds[i].fd, (struct sockaddr *)&addr, addrlen);

        if (result < 0) {
            if (errno != EINPROGRESS) {
                fds[i].fd = DROP_FD; // drop socket immediatly
                open_port[i].status = PORT_UNKNOWN;
                (*port_scanned)++;
                close(sock);
            }
        }
    }

    // Default Timeout (2s)
    struct timeval timeval = {.tv_sec = 2, .tv_usec = 0};

    if (args->timeout) {
        timeval.tv_sec = args->timeout / 1000;
        timeval.tv_usec = (args->timeout % 1000) * 1000;
    }

    int time = (timeval.tv_sec * 1000) + (timeval.tv_usec / 1000);

    size_t finished_nb = 0;

    for (;;) {
        if (finished_nb == nb_ports)
            break;

        int ready = poll(fds, nb_ports, time);

        if (ready < 0) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "[Error] poll failed: %s\n", strerror(errno));
            return -1;
        }

        if (ready == 0)
            break;

        for (size_t i = 0; i < nb_ports; i++) {
            if (fds[i].fd == DROP_FD)
                continue;

            if (fds[i].revents & (POLLOUT | POLLERR | POLLHUP)) {
                int err = 0;
                socklen_t len = sizeof(err);
                getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, &err, &len);

                finished_nb++;
                (*port_scanned)++;
                switch (err) {
                case 0:
                    open_port[i].status = PORT_OPEN;
                    break;
                case ETIMEDOUT:
                    open_port[i].status = PORT_FILTERED;
                    break;
                case EHOSTUNREACH:
                    open_port[i].status = PORT_UNREACHABLE;
                    break;
                case ENETUNREACH:
                    open_port[i].status = PORT_UNREACHABLE;
                    break;
                case ECONNREFUSED:
                    open_port[i].status = PORT_CLOSED;
                    break;
                default:
                    open_port[i].status = PORT_UNKNOWN;
                    break;
                }

                close(fds[i].fd);
                fds[i].fd = DROP_FD;
            }
        }
    }

    for (size_t i = 0; i < nb_ports; i++) {
        if (fds[i].fd != DROP_FD) { // port untreated
            open_port[i].status = PORT_FILTERED;
            (*port_scanned)++;
        }
    }

cleanup:
    for (size_t i = 0; i < fds_nb; i++)
        if (fds[i].fd != DROP_FD)
            close(fds[i].fd);
    return ret;
}