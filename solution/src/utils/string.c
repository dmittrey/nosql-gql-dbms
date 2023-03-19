#include "utils/string.h"

String *string_new()
{
    return my_malloc(String);
}
void string_ctor(String *const str, const char *const val, const size_t size)
{
    char *String_val = my_malloc_array(char, size);

    str->val = strncpy(String_val, val, size);
    str->cnt = size;
}
void string_dtor(String *str)
{
    free(str->val);
    free(str);
}

size_t string_get_size(const String *const str)
{
    return sizeof(char) * str->cnt;
}

/*
Equal - 0
str_2 > str_1 - -1
str_1 > str_2 - 1
*/
int string_cmp(const String *const str_1, const String *const str_2)
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