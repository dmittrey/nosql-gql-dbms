#include <string.h>

#include "utils.h"

#include "memory/string.h"

string_t *string_new()
{
    return my_malloc(string_t);
}
void string_ctor(string_t *const str, const char *const val, const size_t size)
{
    char *string_val = my_malloc_array(char, size);

    str->val = strncpy(string_val, val, size);
    str->cnt = size;
}
void string_dtor(string_t *str)
{
    free(str->val);
    free(str);
}

size_t string_get_size(const string_t *const str)
{
    return sizeof(char) * str->cnt;
}