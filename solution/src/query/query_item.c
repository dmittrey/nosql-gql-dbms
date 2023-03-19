#include "query/query_item.h"

Query_item *query_item_new()
{
    return memset(my_malloc(Query_item), 0, sizeof(Query_item));
}

void query_item_ctor(Query_item *const query_itm, const Json_type type, String *key)
{
    query_itm->query_key = key;
    query_itm->type = type;
}

void query_item_dtor(Query_item *q_itm)
{
    string_dtor(q_itm->query_key);

    if (q_itm->type == TYPE_STRING)
    {
        string_dtor(q_itm->query_val.string_val);
    }

    free(q_itm);
}

bool query_item_check(const Query_item *const query, const Json *const j)
{
    if (string_cmp(query->query_key, j->key) == 0)
    {
        switch (j->type)
        {
        case TYPE_STRING:
            return !string_cmp(j->value.string_val, query->query_val.string_val);
        case TYPE_INT32:
            if (j->value.int32_val == query->query_val.int32_val)
                return true;
        case TYPE_FLOAT:
            if (j->value.float_val == query->query_val.float_val)
                return true;
        case TYPE_BOOL:
            if (j->value.bool_val == query->query_val.bool_val)
                return true;
        }
    }

    return false;
}