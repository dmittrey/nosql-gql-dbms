#include <string.h>

#include "utils.h"

void string_new(string_t *string)
{
    string = my_malloc(string_t);
}

void string_ctor(string_t *const string, const char *const value, const size_t count)
{
    char *value = my_malloc_array(char, count);

    string->val = strncpy(value, string, count);
    string->count = count;
}

void string_dtor(string_t *string)
{
    free(string->val);
    free(string);
}

size_t string_get_size(string_t str)
{
    return str.count * sizeof(char);
}