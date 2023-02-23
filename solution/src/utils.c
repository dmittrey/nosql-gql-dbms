#include <string.h>

#include "utils.h"

string_t *string_new()
{
    return my_malloc(string_t);
}

void string_ctor(string_t *const string, const char *const value, const size_t string_count)
{
    char *string_val = my_malloc_array(char, string_count);

    string->val = strncpy(string_val, value, string_count);
    string->count = string_count;
}

void string_dtor(string_t *string)
{
    free(string->val);
}

size_t string_get_size(string_t str)
{
    return str.count * sizeof(char);
}