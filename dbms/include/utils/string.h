#pragma once

#include "utils/utils.h"

#define STR_INIT(NAME, VAL)      \
    String *NAME = string_new(); \
    string_ctor(NAME, VAL, strlen(VAL))

typedef struct
{
    char *val;
    size_t cnt;
} String;

String *string_new();
void string_ctor(String *const str, const char *const val, const size_t size);
void string_dtor(String *str);

size_t string_get_size(const String *const str);

int string_cmp(const String *const str_1, const String *const str_2);