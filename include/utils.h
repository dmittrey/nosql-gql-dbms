#pragma once

#define my_malloc(X) ((X*) malloc(sizeof(X)))

// Own type of string to simplify handling
typedef struct string_t {
    const char *val;
    const int count; 
} string_t;

enum PerformStatus {
    OK = 0,
    FAILED = 1
};