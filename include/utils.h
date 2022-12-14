#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define my_malloc(T) ((T *)malloc(sizeof(T)))
#define my_malloc_array(T, count) ((T *)malloc(sizeof(T) * count))

typedef uint64_t sectoff_t;
typedef uint64_t fileoff_t;

// Own type of string to simplify handling
typedef struct
{
    const char *val;
    int count;
} string_t;

/*
1 - str_1 > str_2
0 - str_1 == str_2
-1 - str1 != str2
-2 - str_1 < str_2
*/
int compare_string_t(string_t str_1, string_t str_2);

enum PerformStatus
{
    OK = 0,
    FAILED = 1
};

#define OK 0
#define FAILED 1