#include "udp.h"
#include "common.h"
#include "net/socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int udp_create_socket(void)
{
    return socket_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int udp_connect_scan(int sock, const char *target, uint16_t port)
{
    int conn;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (sock < 0) {
        fprintf(stderr, "[Error] Socket creation failed: %s\n",
                strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, target, &addr.sin_addr) <= 0) {
        fprintf(stderr, "[Error] IP adress is invalid: %s\n", target);
        return -1;
    }

    conn = connect(sock, (struct sockaddr *)&addr, addr_len);

    if (conn < 0) {
        fprintf(stderr, "[Error] UDP connection failed: %s\n", strerror(errno));
        return -1;
    }

    uint8_t msg[128];
    generate_random_data(msg, sizeof(msg));

    if (sendto(sock, msg, sizeof(msg), 0, (struct sockaddr *)&addr, addr_len) <
        0) {
        fprintf(stderr, "[Error] sendto failed: %s\n", strerror(errno));
        return -1;
    }

    char buffer[1024];
    ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&addr, &addr_len);

    if (n < 0) {
        if (errno == ECONNREFUSED)
            return ECONNREFUSED;
        else if (errno == EAGAIN)
            return EAGAIN;

        return -1;
    }

    return 0;
}