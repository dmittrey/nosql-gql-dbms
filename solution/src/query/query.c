#include "query/query.h"

static bool query_check_type(const Query *const q, const Type *const type);

Query *query_new()
{
    return memset(my_malloc(Query), 0, sizeof(Query));
}
void query_ctor(Query *q, String *str)
{
    q->type_name = str;
}

void query_dtor(Query *q)
{
    while (q->f_query_itm != NULL)
    {
        Query_item *cur = q->f_query_itm;
        q->f_query_itm = q->f_query_itm->next;
        query_item_dtor(cur);
    }

    free(q);
}

void query_item_add(Query *const q, Query_item *const q_item)
{
    if (q->f_query_itm == NULL)
    {
        q->f_query_itm = q_item;
    }
    else
    {
        Query_item *nxt = q->f_query_itm->next;
        q->f_query_itm->next = q_item;
        q_item->next = nxt;
    }
}

bool query_check(const Query *const query, const Json *const json, const Type *json_type)
{
    const Query_item *cur_query_itm = query->f_query_itm;
    while (cur_query_itm != NULL)
    {
        if (query_item_check(cur_query_itm, json) == false)
            return false;
        
        cur_query_itm = cur_query_itm->next;
    }

    return query_check_type(query, json_type);
}

static bool query_check_type(const Query *const q, const Type *const type)
{
    if (q->type_name == NULL)
        return true;

    if (type->name == NULL)
        return false;

    return string_cmp(q->type_name, type->name) == 0;
}