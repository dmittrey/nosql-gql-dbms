#include "memory/query/query.h"

query_t *query_new()
{
    return memset(my_malloc(query_t), 0, sizeof(query_t));
}

void query_dtor(query_t *query)
{
    free(query);
}

void query_item_add(query_t *const query, query_item_t *const query_item)
{
    if (query->f_query_itm == NULL)
    {
        query->f_query_itm = query_item;
    }
    else
    {
        query_item_t *nxt = query->f_query_itm->next;
        query->f_query_itm->next = query_item;
        query_item->next = nxt;
    }
}

bool query_check_or(const query_t *const query, const json_t *const json)
{
    const query_item_t *cur_query_itm = query->f_query_itm;
    while (cur_query_itm != NULL)
    {
        if (query_item_check(cur_query_itm, json) == true)
            return true;

        cur_query_itm = cur_query_itm->next;
    }

    return false;
}

bool query_check_and(const query_t *const query, const json_t *const json)
{
    const query_item_t *cur_query_itm = query->f_query_itm;
    while (cur_query_itm != NULL)
    {
        if (query_item_check(cur_query_itm, json) == false)
            return false;

        cur_query_itm = cur_query_itm->next;
    }

    return true;
}