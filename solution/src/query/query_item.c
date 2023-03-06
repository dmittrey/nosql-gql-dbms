#include "memory/query/query.h"

query_item_t *query_item_new()
{
    return memset(my_malloc(query_item_t), 0, sizeof(query_item_t));
}

void query_item_ctor(query_item_t *const query_itm, const json_type_t type, string_t *key)
{
    query_itm->query_key = key;
    query_itm->type = type;
}

void query_item_dtor(query_item_t *q_itm)
{
    string_dtor(q_itm->query_key);

    if (q_itm->type == TYPE_STRING)
    {
        string_dtor(q_itm->query_val.string_val);
    }

    free(q_itm);
}

bool query_item_check(const query_item_t *const query, const json_t *const json)
{
    if (string_cmp(query->query_key, json->key) == 0)
    {
        switch (json->type)
        {
        case TYPE_STRING:
            return !string_cmp(json->value.string_val, query->query_val.string_val);
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