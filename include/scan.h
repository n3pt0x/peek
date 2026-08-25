#ifndef SCAN_H
#define SCAN_H

#include "args.h"

void handle_scan(const Arguments *args);
int simple_scan(const Arguments *args);
int scan_range(const Arguments *args);

#endif /* SCAN_H */