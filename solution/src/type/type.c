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

void type_ctor_foff(type_t *const type, string_t *name, list_attr_t *const attr_list, fileoff_t foff)
{
    type_ctor(type, name, attr_list);
    type->foff_ptr = foff;
}

void type_dtor(type_t *type)
{
    string_dtor(type->name);
    list_attr_t_dtor(type->attr_list);
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
    dest->foff_ptr = src->foff_ptr;

    list_attr_t *atr_lst = list_attr_t_new();
    attr_t *cur_atr = src->attr_list->head;
    while (cur_atr != NULL)
    {
        list_attr_t_add(atr_lst, cur_atr);
    }
    dest->attr_list = atr_lst;
}

LIST_DEFINE(type_t, type_dtor, type_cmp);