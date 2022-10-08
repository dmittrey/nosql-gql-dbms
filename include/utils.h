#pragma once

#include <stdbool.h>
#include <stdio.h>

#define my_malloc(X) ((X *)malloc(sizeof(X)))

#define INFO(message) printf("INFO: "); printf(message)
#define ERROR(message) printf("ERROR: "); printf(message)

// Own type of string to simplify handling
struct string_t
{
    const char *val;
    const int count;
};

enum PerformStatus
{
    OK = 0,
    FAILED = 1
};