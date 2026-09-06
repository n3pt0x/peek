#ifndef ARGS_H
#define ARGS_H

#include "peek.h"

int parse_args(int argc, char **argv, Args *args);
void debug_args(const Args *args);
void free_args(Args *args);
void print_usage(const char *program_name);

#endif /* ARGS_H */