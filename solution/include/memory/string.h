#pragma once

#include <stddef.h>

typedef struct
{
    char *val;
    size_t cnt;
} string_t;

string_t *string_new();
void string_ctor(string_t *const, const char *const, const size_t);
void string_dtor(string_t *);

size_t string_get_size(const string_t * const str);