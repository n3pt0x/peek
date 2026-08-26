#include "scan.h"
#include "socket.h"
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
    printf("%s", args->ip);
    return 0;
}

void handle_scan(const Args *args)
{
    if (args->port) {
        char *state = simple_scan(args) ? "open" : "closed";
        printf("Port %d is %s\n", args->port, state);
    }

    if (args->min_port && args->max_port) {
        scan_range(args);
    }
}