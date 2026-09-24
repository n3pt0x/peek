#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>

int socket_create(int domain, int type, int protocol);
int socket_set_timeout(int *sock, int timeout_ms);
int socket_set_ttl(int *sock, int ttl);
int set_nonblocking(int sock);

#endif /* SOCKET_H */