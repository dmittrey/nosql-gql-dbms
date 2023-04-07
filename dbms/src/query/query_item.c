#include <assert.h>

#include "query/query_item.h"
#include "query/path.h"

Query_item *query_item_new()
{
    return my_calloc(Query_item);
}

void query_item_ctor(Query_item *const q_itm, const Json_type j_type, List_Path *key, Cmp cmp_op)
{
    q_itm->key = key;
    q_itm->type = j_type;
    q_itm->cmp_op = cmp_op;
}
void query_item_dtor(Query_item *q_itm)
{
    list_Path_dtor(q_itm->key);

    if (q_itm->type == TYPE_STRING)
        string_dtor(q_itm->query_val.string_val);

    free(q_itm);
}

bool query_item_check(const Query_item *const q_itm, const Json *const j)
{
    Path *cur_key = q_itm->key->head;
    const Json *cur_json = j;
    while (cur_key != NULL) // Drill down to field
    {
        cur_json = json_son_by_key(cur_json, cur_key->name);
        if (cur_json == NULL)
            return false;

        cur_key = cur_key->next;
    }

    switch (q_itm->type)
    {
    case TYPE_STRING:
        switch (q_itm->cmp_op)
        {
        case IN:
            return (strstr(cur_json->value.string_val->val, q_itm->query_val.string_val->val) != NULL);
        case EQ:
            return (strcmp(cur_json->value.string_val->val, q_itm->query_val.string_val->val) == 0);
        default:
            assert("Operation doesn't exist!");
        }
    case TYPE_INT32:
        switch (q_itm->cmp_op)
        {
        case EQ:
            return cur_json->value.int32_val == q_itm->query_val.int32_val;
        case GT:
            return cur_json->value.int32_val > q_itm->query_val.int32_val;
        case GE:
            return cur_json->value.int32_val >= q_itm->query_val.int32_val;
        case LT:
            return cur_json->value.int32_val < q_itm->query_val.int32_val;
        case LE:
            return cur_json->value.int32_val <= q_itm->query_val.int32_val;
        default:
            assert("Operation doesn't exist!");
        }
    case TYPE_FLOAT:
        switch (q_itm->cmp_op)
        {
        case EQ:
            return cur_json->value.float_val == q_itm->query_val.float_val;
        case GT:
            return cur_json->value.float_val > q_itm->query_val.float_val;
        case GE:
            return cur_json->value.float_val >= q_itm->query_val.float_val;
        case LT:
            return cur_json->value.float_val < q_itm->query_val.float_val;
        case LE:
            return cur_json->value.float_val <= q_itm->query_val.float_val;
        default:
            assert("Operation doesn't exist!");
        }
    case TYPE_BOOL:
        switch (q_itm->cmp_op)
        {
        case EQ:
            return cur_json->value.bool_val == q_itm->query_val.bool_val;
        default:
            assert("Operation doesn't exist!");
        }
    default:
        return false;
    }
}