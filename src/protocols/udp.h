#ifndef UDP_H
#define UDP_H

#include <stdint.h>

int udp_connect_scan(int sock, const char *target, uint16_t port);
int udp_create_socket(void);

#endif /* UDP_H */