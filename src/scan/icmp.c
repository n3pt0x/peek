#include "icmp.h"
#include "common.h"
#include "utils.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct header_icmp {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
} __attribute__((packed));

static int dns_lookup(EchoRequest *req)
{
    struct addrinfo hints = {0};
    struct addrinfo *result;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    if (getaddrinfo(req->target, NULL, &hints, &result) < 0) {
        return -1;
    }

    memcpy(&req->addr, result->ai_addr, result->ai_addrlen);
    req->addr_len = result->ai_addrlen;

    freeaddrinfo(result);

    return 0;
}

int icmp_create_sock(EchoRequest *req)
{
    req->sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (req->sock < 0) {
        return -1;
    }

    return 0;
}

int icmp_set_timeout(EchoRequest *req, int timeout)
{
    if (req->sock < 0) {
        return -1;
    }

    if (!is_valid_timeout(timeout)) {
        fprintf(stderr, "[Error] Timeout must be between 1 and 200 seconds.");
        return -1;
    }

    req->timeout = timeout;

    struct timeval tv = ms_to_timeval(req->timeout);
    if (setsockopt(req->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        return -1;
    }

    return 0;
}

int icmp_set_ttl(EchoRequest *req, int ttl)
{
    if (req->sock < 0) {
        return -1;
    }

    if (!is_valid_ttl(ttl)) {
        fprintf(stderr, "[Error] TTL must be between 1 and 255.");
        return -1;
    }

    req->ttl = ttl;
    return setsockopt(req->sock, IPPROTO_ICMP, IP_TTL, (const void *)&req->ttl,
                      sizeof(req->ttl));
}

int icmp_build_echo(EchoRequest *req, uint8_t *packet, size_t *packet_len,
                    size_t payload_len)
{
    if (!packet) {
        return -1;
    }

    /* struct ICMP */
    struct header_icmp *header = (struct header_icmp *)packet;
    header->type = 8;
    header->code = 0;
    header->checksum = 0;
    header->id = htons(getpid());
    header->sequence = htons(1);

    /* Random Payload */
    uint8_t *payload = packet + ICMP_HEADER_LEN;
    for (uint32_t i = 0; i < payload_len; i++) {
        payload[i] = rand() % 256;
    }

    *packet_len = ICMP_HEADER_LEN + payload_len;
    header->checksum = htons(checksum(packet, *packet_len));

    int status = dns_lookup(req);
    if (status != 0) {
        fprintf(stderr, "[Error] DNS resolver failed: %s.\n",
                gai_strerror(status));
        return -1;
    }

    return 0;
}

int icmp_send_packet(const EchoRequest *req, uint8_t *packet, size_t packet_len)
{
    if (!req || req->sock < 0 || !packet || packet_len == 0) {
        return -1;
    }

    ssize_t sent = sendto(req->sock, packet, packet_len, 0,
                          (const struct sockaddr *)&req->addr, req->addr_len);

    if (sent < 0) {
        perror("sendto");
        return -1;
    }

    if (sent != (ssize_t)packet_len) {
        fprintf(stderr, "[Error] Sent %zd bytes, expected %zd", sent, packet_len);
        return -1;
    }

    return 0;
}
