#pragma once

#define my_malloc(X) ((X*) malloc(sizeof(X)))

// Own type of string to simplify handling
struct string_t {
    const char *val;
    const int count; 
};

enum PerformStatus {
    OK = 0,
    FAILED = 1
};