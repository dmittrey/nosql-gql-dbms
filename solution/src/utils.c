#include <string.h>

#include "utils.h"

string_t string_ctor(const char *string, size_t count)
{
    return (string_t) {.count = count, .val = string};
}

size_t string_get_size(string_t str)
{
    return str.count * sizeof(char);
}