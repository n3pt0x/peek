#include "scan.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int simple_scan(const Arguments *args)
{
    int sock, conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    sock = socket(AF_INET, args->type, 0);
    if (sock < 0)
    {
        fprintf(stderr, "[Error] Socket creation failed %s\n", strerror(errno));
        goto close;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(args->port);
    if (inet_pton(AF_INET, args->ip, &(addr.sin_addr)) <= 0)
    {
        fprintf(stderr, "[Error] IP adress is invalid: %s\n", args->ip);
        goto close;
    }

    conn = connect(sock, (const struct sockaddr *)&addr, addr_len);

    close(sock);
    return (conn == 0);
close:
    if (sock >= 0) close(sock);
    return 0;
}

int scan_range(const Arguments *args)
{
    printf("%s" ,args->ip);
    return 0;
}

void handle_scan(const Arguments *args)
{
    if (args->port)
    {
        char *state = simple_scan(args) ? "open" : "closed";
        printf("Port %d is %s\n", args->port, state);
    }

    if (args->port_min && args->port_max)
    {
        scan_range(args);
    }
}