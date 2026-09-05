#ifndef ICMP_H
#define ICMP_H

#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>

#define ICMP_ECHO_REPLY 0x0
#define ICMP_ECHO_REQUEST 0x8
#define ICMP_HEADER_LEN 8

typedef struct EchoRequest {
    char target[256];
    int sock;
    uint8_t ttl;
    int timeout_ms;
    int sequence;
    struct sockaddr_storage addr;
    socklen_t addr_len;
} EchoRequest;

int icmp_create_sock(EchoRequest *req);
int icmp_set_timeout(EchoRequest *req, int timeout);
int icmp_set_ttl(EchoRequest *req, int ttl);
int icmp_build_echo(EchoRequest *req, uint8_t *packet, size_t *packet_len, size_t payload_len);
int icmp_send_packet(const EchoRequest *req, uint8_t *packet, size_t packet_len);
int icmp_recv_reply(EchoRequest *req, uint8_t *buffer, size_t buffer_len);

#endif /* ICMP_H */