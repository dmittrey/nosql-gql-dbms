#include "memory/attribute/obj_type.h"

obj_type_t *obj_type_new()
{
    return calloc(1, sizeof(obj_type_t));
}

void obj_type_ctor(obj_type_t *const obj_type, string_t *const key, attr_col_t *const attrs)
{
    obj_type->key = key;
    obj_type->attrs = attrs;
}

void obj_type_dtor(obj_type_t * obj_type)
{   
    string_dtor(obj_type->key);
    attr_col_dtor(obj_type->attrs);
    free(obj_type);
}

int obj_type_cmp(obj_type_t *const ot, json_t *const json)
{
    // Пройтись по всем сыновьям и сравнить чтобы 1 в 1 были поля
    // Добавить в json метод сравнения с attr_t
}