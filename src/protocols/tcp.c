#include "tcp.h"
#include "net/socket.h"
#include "utils/utils.h"
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int scan_single_tcp_port(const Args *args, uint16_t port)
{
    int sock, conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (!args->s_type || args->s_type != SOCK_STREAM) {
        fprintf(stderr, "[Error] TCP socket type is not valid\n");
        return -1;
    }

    sock = socket_create(AF_INET, args->s_type, 0);

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
    return conn;
}

int scan_range_port(const Args *args, StatusPort *open_port, int *count)
{
    if (!is_valid_range_port(args->min_port, args->max_port)) {
        return -1;
    }

    for (int i = 0, port = args->min_port; port < args->max_port;
         port++, i++, (*count)++) {
        if (scan_single_tcp_port(args, port) < 0) {
            if (errno == ETIMEDOUT)
                open_port[i].status = 1;
            else if (errno == ECONNREFUSED)
                continue;
            else
                return -1;
        } else {
            open_port[i].status = 0;
        }

        open_port[i].port = port;
    }

    return 0;
}