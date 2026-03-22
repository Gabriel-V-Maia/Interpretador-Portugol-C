#include <stdio.h>
#include_next <stdlib.h>

#ifndef OPERATIONS_H
#define OPERATIONS_H

#define PANIC(fmt, ...) \
    do { \
        fprintf(stderr, "erro: " fmt "\n", ##__VA_ARGS__); \
        exit(1); \
    } while(0)

#define PANIC_IF(cond, fmt, ...) \
    do { \
        if (cond) PANIC(fmt, ##__VA_ARGS__); \
    } while(0)

#endif
