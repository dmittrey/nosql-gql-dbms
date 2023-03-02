#include "utils.h"
#include "string.h"

#include "memory/json/json.h"

typedef struct
{
    string_t *query_key;
    union
    {
        int32_t int32_val;
        float float_val;
        string_t *string_val;
        bool bool_val;
    } query_val;
} query_t;

bool query_check(const query_t *const, const json_t *const);