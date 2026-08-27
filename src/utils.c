#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>

bool is_valid_ip(const char *ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr));
}
