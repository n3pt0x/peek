#include "scan.h"
#include "flags.h"
#include "socket.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int simple_scan(const Args *args)
{
    int sock, conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (args->s_type == SOCK_DGRAM) {
        sock = init_socket(args->s_type);
    } else {
        sock = init_socket(SOCK_STREAM);
    }

    if (sock < 0) {
        fprintf(stderr, "[Error] Socket creation failed %s\n", strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(args->port);
    if (inet_pton(AF_INET, args->ip, &(addr.sin_addr)) <= 0) {
        fprintf(stderr, "[Error] IP adress is invalid: %s\n", args->ip);
        return -1;
    }

    conn = connect(sock, (const struct sockaddr *)&addr, addr_len);

    close(sock);
    return (conn == 0);
}

int scan_range(const Args *args)
{
    if (!is_valid_port(args->min_port) || !is_valid_port(args->max_port) ) {
        return -1;
    }

    return 0;
}

int handle_scan(const Args *args)
{
    if (args->flags & SCAN_RANGE)
        return scan_range(args);

    if (args->port) {
        int state = simple_scan(args);
        printf("Port %d is %s\n", args->port, (state == 0) ? "open" : "closed");
        return state;
    }

    return -1;
}