#include "utils.h"
#include "peek.h"
#include <stdio.h>

void print_port_status(uint16_t port, PortState port_state, uint32_t flags)
{
    switch (port_state) {
    case PORT_OPEN:
        printf("Port %d is open\n", port);
        break;
    case PORT_FILTERED:
        printf("Port %d is filtered\n", port);
        break;
    case PORT_UNREACHABLE:
        printf("Port %d is unreachable/filtered\n", port);
        break;
    case PORT_CLOSED:
        if (flags & SCAN_VERBOSE)
            printf("Port %d is closed\n", port);
        break;
    default:
        if (flags & SCAN_VERBOSE)
            printf("Port %d is closed\n", port);
        break;
    }
}