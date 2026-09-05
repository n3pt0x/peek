#include "icmp.h"
#include "common.h"
#include "utils/utils.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct icmphdr {
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
    req->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

    if (req->sock < 0) {
        return -1;
    }

    return 0;
}

int icmp_set_timeout(EchoRequest *req, int timeout_ms)
{
    if (req->sock < 0) {
        return -1;
    }

    if (!is_valid_timeout(timeout_ms)) {
        fprintf(stderr, "[Error] Timeout must be between 1 and 200 seconds\n");
        return -1;
    }

    req->timeout_ms = timeout_ms;

    struct timeval tv = ms_to_timeval(req->timeout_ms);
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
        fprintf(stderr, "[Error] TTL must be between 1 and 255\n");
        return -1;
    }

    req->ttl = ttl;
    return setsockopt(req->sock, IPPROTO_ICMP, IP_TTL, (const void *)&req->ttl,
                      sizeof(req->ttl));
}

int icmp_build_echo(EchoRequest *req, uint8_t *packet, size_t *packet_len, size_t payload_len)
{
    if (!packet) {
        return -1;
    }

    /* struct ICMP */
    struct icmphdr *header = (struct icmphdr *)packet;
    header->type = ICMP_ECHO_REQUEST;
    header->code = 0;
    header->checksum = 0;
    header->id = 0;
    header->sequence = htons(req->sequence);

    /* Random Payload */
    uint8_t *payload = packet + ICMP_HEADER_LEN;
    for (uint32_t i = 0; i < payload_len; i++) {
        payload[i] = rand() % 256;
    }

    *packet_len = ICMP_HEADER_LEN + payload_len;

    int status = dns_lookup(req);
    if (status != 0) {
        fprintf(stderr, "[Error] DNS resolver failed: %s\n",
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

int icmp_recv_reply(EchoRequest *req, uint8_t *buffer, size_t buffer_len)
{
    memset(buffer, 0, buffer_len);

    struct sockaddr_storage src_addr;
    socklen_t src_addr_len = sizeof(src_addr);

    ssize_t n = recvfrom(req->sock, buffer, buffer_len - 1, 0, (struct sockaddr *)&src_addr, &src_addr_len);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 1;
        }
        fprintf(stderr, "[Error] Recv echo request has failed: %s\n", strerror(errno));
        return -1;
    }

    if ((size_t)n < sizeof(struct icmphdr)) {
        fprintf(stderr, "[Error] Packet too short\n");
        return -1;
    }

    struct icmphdr *reply = (struct icmphdr *)buffer;

    if (reply->type != ICMP_ECHO_REPLY) {
        fprintf(stderr, "[Error] Not a Echo Request type (type=%d)\n", reply->type);
        return -1;
    }

    return 0;
}
