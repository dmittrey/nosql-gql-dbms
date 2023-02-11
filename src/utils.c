#include "utils.h"

size_t string_get_size(string_t str)
{
    return str.count * sizeof(char);
}