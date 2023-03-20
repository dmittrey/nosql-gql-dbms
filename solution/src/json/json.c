#include "json/json.h"

Json *json_new()
{
    return my_calloc(Json);
}

void json_ctor(Json *const Json, const Json_type type, const char *const key_val, const size_t key_size)
{
    Json->key = string_new();
    string_ctor(Json->key, key_val, key_size);

    Json->type = type;
}

void json_dtor(Json *j)
{
    // Change dad's son ptr to Json bro
    if (j->dad != NULL)
    {
        j->dad->son = j->bro;
    }

    // Free all children
    Json *cur_son = j->son;
    Json *next_son = NULL;
    while (cur_son != NULL)
    {
        next_son = cur_son->bro;
        json_dtor(cur_son);
        cur_son = next_son;
    }

    // Free Json
    if (j->key != NULL)
        string_dtor(j->key);
    if (j->type == TYPE_STRING)
        string_dtor(j->value.string_val);
    free(j);
}

void json_dtor_with_bro(Json *j)
{
    while (j != NULL)
    {
        Json *bro = j->bro;
        json_dtor(j);
        j = bro;
    }
}

void *json_val_ptr(const Json *const j)
{
    switch (j->type)
    {
    case TYPE_OBJECT:
        return NULL;
    case TYPE_STRING:
        return j->value.string_val->val;
    default:
        return (void *)&j->value;
    }
}

size_t json_val_size(const Json *const j)
{
    switch (j->type)
    {
    case TYPE_OBJECT:
        return 0;
    case TYPE_STRING:
        return string_get_size(j->value.string_val);
    default:
        return sizeof(j->value);
    }
}

void json_add_bro(Json *const j, Json *const bro)
{
    Json *cur_Json = j;
    while (cur_Json->bro != NULL)
    {
        cur_Json = cur_Json->bro;
    }

    cur_Json->bro = bro;
}

void json_add_son(Json *const j, Json *const son)
{
    if (j->son == NULL)
    {
        j->son = son;
    }
    else
    {
        Json *cur_son = j->son;
        while (cur_son->bro != NULL)
        {
            cur_son = cur_son->bro;
        }

        cur_son->bro = son;
    }
}

void json_add_nxt(Json *const j, Json *const next)
{
    if (j != NULL)
    {
        struct Json *nxt = j->next;
        j->next = next;
        next->next = nxt;
    }
}

/*
Equal - 0
j1 < j2 - -1
j1 > j2 - 1
*/
int json_cmp(const Json *const j1, const Json *const j2)
{
    if (j1->type > j2->type)
    {
        return 1;
    }
    else if (j1->type < j2->type)
    {
        return -1;
    }
    else
    {
        int key_cmp = string_cmp(j1->key, j2->key);
        if (key_cmp != 0)
            return key_cmp;

        switch (j1->type)
        {
        case TYPE_STRING:
            return string_cmp(j1->value.string_val, j2->value.string_val);
        case TYPE_INT32:
            if (j1->value.int32_val == j2->value.int32_val)
                return 0;
            else
                return 1;
        case TYPE_FLOAT:
            if (j1->value.float_val == j2->value.float_val)
                return 0;
            else
                return 1;
        case TYPE_BOOL:
            if (j1->value.bool_val == j2->value.bool_val)
                return 0;
            else
                return 1;
        case TYPE_OBJECT:
            return 0;
        }
    }
    return 1;
}

int json_cmp_wth_foff(const Json *const j1, const Json *const j2)
{
    return memcmp(&j1->foff, &j2->foff, sizeof(Fileoff));
}

/*
Type fields are required
*/
bool json_is_apply_type(const Json *const j, const Type *const t)
{
    if (j->type == TYPE_OBJECT)
    {
        Attr *cur_atr = t->attr_list->head;

        Json *cur_son = j->son;
        while (cur_son != NULL && cur_atr != NULL)
        {
            if (string_cmp(cur_atr->name, cur_son->key) == 0)
            {
                if (cur_atr->type != cur_son->type)
                {
                    return FAILED;
                }
                else
                {
                    cur_atr = cur_atr->next;
                }
            }

            cur_son = cur_son->next;
        }

        // Что-то не вошло в object Json
        if (cur_atr != NULL)
        {
            return FAILED;
        }

        return OK;
    }
    else
    {
        return FAILED;
    }
}