#include <string.h>

#include "utils.h"

string_t string_ctor(const char *string)
{
    return (string_t) {.count = strlen(string), .val = string};
}

size_t string_get_size(string_t str)
{
    return str.count * sizeof(char);
}