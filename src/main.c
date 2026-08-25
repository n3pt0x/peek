#include "scan.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s <ip> -p <port>\n", argv[0]);
        return 1;
    }

    /* Arguments */
    Arguments args = {0};
    args.s_type = SOCK_STREAM;
    parse_arguments(argv, &args);

    if (!is_valid_ip(argv[1]))
    {
        printf("Format of ip address is not valid\nUsage %s <ip> -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Scan */
    handle_scan(&args);
    return 0;
}