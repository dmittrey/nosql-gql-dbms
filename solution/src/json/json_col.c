#include <string.h>

#include "memory/json/json_col.h"

json_col_t *json_col_new()
{
    return memset(my_malloc(json_col_t), 0, sizeof(json_col_t));
}

void json_col_ctor(json_col_t *const collection, json_col_t *other)
{
    collection->f_json = other->f_json;
    collection->l_json = other->l_json;
    collection->count = other->count;
}

void json_col_dtor(json_col_t *collection)
{
    while (collection->f_json != NULL)
    {
        json_t *cur = collection->f_json;
        collection->f_json = collection->f_json->next;
        json_dtor(cur);
    }

    free(collection);
}

void json_col_add(json_col_t *collection, json_t *json)
{
    if (collection->f_json == NULL)
    {
        collection->f_json = json;
        collection->l_json = json;
    }
    else
    {
        collection->l_json->next = json;
        collection->l_json = json;
    }

    collection->count += 1;
}

void json_col_del_nxt(json_col_t *json_col, json_t *json)
{
    json_t *nxt = json->next;
    if (nxt != NULL)
    {
        json->next = nxt->next;
        json_dtor(nxt);
    }
    else
    {
        json->next = NULL;
        json_col->l_json = json;
    }

    json_col->count -= 1;
}

void json_col_del_fst(json_col_t *json_col)
{
    if (json_col->f_json == json_col->l_json)
    {
        json_col->f_json = NULL;
        json_col->l_json = NULL;
    }
    else
    {
        json_t *json_nxt = json_col->f_json->next;
        json_dtor(json_col->f_json);
        json_col->f_json = json_nxt;
    }

    json_col->count -= 1;
}

bool json_col_add_lk_set(json_col_t *collection, json_t *json)
{

    json_t *cur = collection->f_json;

    if (cur == NULL)
    {
        collection->f_json = json;
        collection->l_json = json;
        collection->count += 1;
        return true;
    }

    while (cur != NULL)
    {
        // Если нашли, то выходим не добавив
        if (json_cmp(json, cur) == 0)
            return false;

        if (cur->next == NULL)
        {
            cur->next = json;
            break;
        }

        cur = cur->next;
    }

    collection->l_json = json;
    collection->count += 1;
    return true;
}