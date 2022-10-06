#pragma once

#include <stdbool.h>

#define my_malloc(X) ((X*) malloc(sizeof(X)))

// extern bool log_enabled = true;

// #if log_enabled == true
    #define LOG(message) printf(message)
// #else
    // #define LOG(message) 
// #endif

// Own type of string to simplify handling
struct string_t {
    const char *val;
    const int count; 
};

enum PerformStatus {
    OK = 0,
    FAILED = 1
};