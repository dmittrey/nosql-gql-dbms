#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define my_malloc(X) ((X *)malloc(sizeof(X)))

#define INFO(message) printf("INFO: "); printf(message)
#define ERROR(message) printf("ERROR: "); printf(message)

// Own type of string to simplify handling
struct string_t
{
    const char *val;
    const int count;
};

/*
1 - str_1 > str_2
0 - str_1 == str_2
-1 - str1 != str2
-2 - str_1 < str_2
*/
int compare_string_t(struct string_t str_1, struct string_t str_2);

enum PerformStatus
{
    OK = 0,
    FAILED = 1
};

#define FAILED 1
#define OK 0