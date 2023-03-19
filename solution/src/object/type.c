#include "json/object/type.h"
#include "json/object/type_ent.h"
#include "json/object/attr_ent.h"

Type *type_new()
{
    return calloc(1, sizeof(Type));
}

void type_ctor(Type *const t, String *name, List_Attr *const attr_list)
{
    t->name = name;
    t->attr_list = attr_list;
}

void type_dtor(Type *t)
{
    if (t->name != NULL)
    {
        string_dtor(t->name);
    }

    if (t->attr_list != NULL)
    {
        list_Attr_dtor(t->attr_list);
    }

    free(t);
}

int type_cmp(const Type *const t1, const Type *const t2)
{
    return string_cmp(t1->name, t2->name);
}
void type_cpy(Type *const dest, Type *const src)
{
    dest->name = string_new();
    string_ctor(dest->name, src->name->val, src->name->cnt);

    dest->attr_list = list_Attr_new();
    Attr *cur_atr = src->attr_list->head;
    while (cur_atr != NULL)
    {
        type_add_atr(dest, cur_atr);
        cur_atr = cur_atr->next;
    }

    src->attr_list->head = NULL;
    src->attr_list->tail = NULL;
    src->attr_list->count = 0;
}

void type_add_atr(Type *const t, Attr *const a)
{
    list_Attr_add(t->attr_list, a);
}

size_t type_ph_sz(const Type *const t)
{
    return type_itm_sz(t) + type_rec_sz(t);
}

size_t type_itm_sz(const Type *const t)
{
    size_t size = sizeof(Type_entity);
    size += t->attr_list->count * sizeof(Attr_entity);
    return size;
}

size_t type_rec_sz(const Type *const t)
{
    size_t size = t->name->cnt * sizeof(char);

    Attr *cur_atr = t->attr_list->head;
    while (cur_atr != NULL)
    {
        size += cur_atr->name->cnt * sizeof(char);
        cur_atr = cur_atr->next;
    }

    return size;
}

LIST_DEFINE(Type, type_dtor, type_cmp);