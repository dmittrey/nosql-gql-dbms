#include <string.h>

#include "memory/json/it_json.h"

it_json_t *it_json_new()
{
    return memset(my_malloc(it_json_t), 0, sizeof(it_json_t));
}

void it_json_ctor(it_json_t *const it_json, json_t * json)
{
    it_json->json = json;
    it_json->next = NULL;
}
void it_json_dtor(it_json_t *it_json)
{
    json_dtor(it_json->json);
    free(it_json);
}

void it_json_add_nxt(it_json_t *const it_json, it_json_t * next)
{
    if (it_json->json == NULL)
    {
        it_json->json = next->json;
    }
    else
    {
        struct it_json_t *nxt = it_json->next;
        it_json->next = next;
        next->next = nxt;
    }
}