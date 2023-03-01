#pragma once

#include <stddef.h>

#define STR_INIT(NAME, VAL)        \
    string_t *NAME = string_new(); \
    string_ctor(NAME, VAL, strlen(VAL))

typedef struct
{
    char *val;
    size_t cnt;
} string_t;

string_t *string_new();
void string_ctor(string_t *const, const char *const, const size_t);
void string_dtor(string_t *);

size_t string_get_size(const string_t * const str);

int string_cmp(const string_t * const str_1, const string_t * const str_2);