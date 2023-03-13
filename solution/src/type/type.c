#include "memory/type/type.h"

type_t *type_new()
{
    return calloc(1, sizeof(type_t));
}

void type_ctor(type_t *const type, string_t *name, list_attr_t *const attr_list)
{
    type->name = name;
    type->attr_list = attr_list;
}

void type_ctor_soff(type_t *const type, string_t *name, list_attr_t *const attr_list, fileoff_t soff)
{
    type_ctor(type, name, attr_list);
    type->soff_ptr = soff;
}

void type_dtor(type_t *type)
{
    if (type->name != NULL)
    {
        string_dtor(type->name);
    }

    if (type->attr_list != NULL)
    {
        list_attr_t_dtor(type->attr_list);
    }

    free(type);
}

int type_cmp(const type_t *const t1, const type_t *const t2)
{
    return string_cmp(t1->name, t2->name);
}
void type_cpy(type_t *const dest, type_t *const src)
{
    dest->name = string_new();
    string_ctor(dest->name, src->name->val, src->name->cnt);
    dest->soff_ptr = src->soff_ptr;

    dest->attr_list = list_attr_t_new();
    attr_t *cur_atr = src->attr_list->head;
    while (cur_atr != NULL)
    {
        type_add_atr(dest, cur_atr);
        cur_atr = cur_atr->next;
    }

    src->attr_list->head = NULL;
    src->attr_list->tail = NULL;
    src->attr_list->count = 0;
}

void type_add_atr(type_t *const t, attr_t *const a)
{
    list_attr_t_add(t->attr_list, a);
}

size_t type_ph_sz(const type_t *const type)
{
    size_t size = type->name->cnt * sizeof(char);

    attr_t *cur_atr = type->attr_list->head;
    while (cur_atr != NULL)
    {
        size += cur_atr->name->cnt * sizeof(char);
        cur_atr = cur_atr->next;
    }

    return size;
}

LIST_DEFINE(type_t, type_dtor, type_cmp);