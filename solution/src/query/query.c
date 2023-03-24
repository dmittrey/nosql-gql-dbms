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

// Iterate over bro's and find all consitional satisfies
/*
Проходимся по всем условиям, для каждого условия:
    - Проходимся по json и всем его братьям, если удовлетворяем, идем к след условию
*/
bool query_check(const Query *const q, const Json *const json, const Type *json_type)
{
    const Query_item *cur_query_itm = q->f_query_itm;
    while (cur_query_itm != NULL)
    {
        const Json *temp = json->son;
        bool is_satisfies = false;

        while (temp != NULL)
        {
            if (query_item_check(cur_query_itm, temp) == true)
            {
                is_satisfies = true;
                break;
            }

            temp = temp->bro;
        }

        // Если горизонтально не нашли объекта, который удовлетворяет условию
        if (is_satisfies == false)
            return false;

        cur_query_itm = cur_query_itm->next;
    }

    return query_check_type(q, json_type);
}

static bool query_check_type(const Query *const q, const Type *const type)
{
    if (q->type_name == NULL)
        return true;

    if (type->name == NULL)
        return false;

    return string_cmp(q->type_name, type->name) == 0;
}