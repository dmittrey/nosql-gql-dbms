#include "memory/query/query.h"

bool query_check(const query_t *const query, const json_t *const json)
{
    if (string_cmp(query->query_key, json->key) == 0)
    {
        switch (json->type)
        {
        case TYPE_STRING:
            return string_cmp(json->value.string_val, query->query_val.string_val);
        case TYPE_INT32:
            if (json->value.int32_val == query->query_val.int32_val)
                return true;
        case TYPE_FLOAT:
            if (json->value.float_val == query->query_val.float_val)
                return true;
        case TYPE_BOOL:
            if (json->value.bool_val == query->query_val.bool_val)
                return true;
        }
    }

    return false;
}