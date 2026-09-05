#include "args.h"
#include "scan.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: %s <ip> -p <port>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    int ret = EXIT_FAILURE;

    /* Arguments */
    Args args = {0};
    
    if (parse_args(argc, argv, &args) < 0) {
        goto arg_failure;
    }

    /* Scan */
    if (handle_scan(&args) < 0) {
        goto arg_failure;
    }
    
    ret = EXIT_SUCCESS;

arg_failure:
    free_args(&args);
    return ret;
}