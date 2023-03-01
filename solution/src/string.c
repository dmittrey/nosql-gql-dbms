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

/*
Equal - 0
str_2 > str_1 - -1
str_1 > str_2 - 1
*/
int string_cmp(const string_t *const str_1, const string_t *const str_2)
{
    if (str_1->cnt > str_2->cnt)
    {
        return 1;
    }
    else if (str_1->cnt < str_2->cnt)
    {
        return -1;
    }
    else
    {
        return strncmp(str_1->val, str_2->val, str_1->cnt);
    }
}