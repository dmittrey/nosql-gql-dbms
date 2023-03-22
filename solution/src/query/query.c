#include "query/query.h"

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

bool query_check_or(const Query *const q, const Json *const j)
{
    const Query_item *cur_query_itm = q->f_query_itm;
    while (cur_query_itm != NULL)
    {
        if (query_item_check(cur_query_itm, j) == true)
            return true;

        cur_query_itm = cur_query_itm->next;
    }

    return false;
}

// Iterate over bro's and find all consitional satisfies
/*
Проходимся по всем условиям, для каждого условия:
    - Проходимся по json и всем его братьям, если удовлетворяем, идем к след условию
*/
bool query_check_and(const Query *const q, const Json *const dad_json)
{
    const Query_item *cur_Query_itm = q->f_query_itm;
    while (cur_Query_itm != NULL)
    {
        const Json *temp = dad_json->son;
        bool is_satisfies = false;

        while (temp != NULL)
        {
            if (query_item_check(cur_Query_itm, temp) == true)
            {
                is_satisfies = true;
                break;
            }

            temp = temp->bro;
        }

        // Если горизонтально не нашли объекта, который удовлетворяет условию
        if (is_satisfies == false)
            return false;

        cur_Query_itm = cur_Query_itm->next;
    }

    return true;
}

bool query_check_type(const Query *const q, const Type *const type)
{
    if (q->type_name == NULL)
        return true;

    if (type->name == NULL)
        return false;

    return string_cmp(q->type_name, type->name) == 0;
}