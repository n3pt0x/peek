#include "scan.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: %s <ip> -p <port>\n", argv[0]);
        return 1;
    }

    /* Arguments */
    Args args = {0};
    args.s_type = SOCK_STREAM;
    if (parse_args(argc, argv, &args) != 0) {
        return EXIT_FAILURE;
    }

    /* Scan */
    handle_scan(&args);
    return EXIT_SUCCESS;
}