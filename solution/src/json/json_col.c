#include <string.h>

#include "memory/json/json_col.h"

json_col_t *json_col_new()
{
    return memset(my_malloc(json_col_t), 0, sizeof(json_col_t));
}

void json_col_ctor(json_col_t *const collection, json_col_t *other)
{
    collection->f_json = other->f_json;
}

void json_col_dtor(json_col_t *collection)
{
    while (collection->f_json != NULL)
    {
        json_dtor(collection->f_json);
        collection->f_json = collection->f_json->next;
    }

    free(collection);
}

void json_col_add(json_col_t *collection, json_t *json)
{
    if (collection->f_json == NULL)
    {
        collection->f_json = json;
    }
    else
    {
        json_add_nxt(collection->f_json, json);
    }
}