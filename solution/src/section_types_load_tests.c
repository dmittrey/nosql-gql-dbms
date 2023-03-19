#include <assert.h>

#include "section/types.h"
#include "section/types_p.h"

static const char *test_file_name = "test.bin";

/*
1) Скачивание пустой секции
2) Скачивание типа без атрибутов
3) Скачивание типа со всеми атрибутами
*/

Status SectionTypes_LoadEmptySection_LoadNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    List_Pair_Sectoff_Type *t_list = list_Pair_Sectoff_Type_new();
    sect_types_load(types, t_list);

    assert(t_list->count == 0);

    sect_types_dtor(types);
    list_Pair_Sectoff_Type_dtor(t_list);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_LoadTypeWithoutAttributes_LoadType()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(V_type, "V");

    Sectoff wrt_adr;
    sect_types_write(types, V_type, &wrt_adr);

    List_Pair_Sectoff_Type *t_list = list_Pair_Sectoff_Type_new();
    sect_types_load(types, t_list);

    assert(type_cmp(t_list->head->s, V_type) == 0);
    assert(t_list->count == 1);

    sect_types_dtor(types);
    type_dtor(V_type);
    list_Pair_Sectoff_Type_dtor(t_list);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_LoadTypeWithAllAttributes_LoadType()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(V_type, "V");

    ATR_INIT(bool_atr, "bool", TYPE_BOOL);
    type_add_atr(V_type, bool_atr);
    ATR_INIT(int32_atr, "int32", TYPE_INT32);
    type_add_atr(V_type, int32_atr);
    ATR_INIT(str_atr, "str", TYPE_STRING);
    type_add_atr(V_type, str_atr);
    ATR_INIT(float_atr, "float", TYPE_FLOAT);
    type_add_atr(V_type, float_atr);

    TYPE_INIT(K_type, "K");

    Sectoff wrt_adr;
    sect_types_write(types, V_type, &wrt_adr);
    sect_types_write(types, K_type, &wrt_adr);

    List_Pair_Sectoff_Type *t_list = list_Pair_Sectoff_Type_new();
    sect_types_load(types, t_list);

    assert(type_cmp(t_list->head->s, V_type) == 0);
    assert(attr_cmp(t_list->head->s->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t_list->head->s->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t_list->head->s->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t_list->head->s->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t_list->head->s->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t_list->head->s->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t_list->head->s->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t_list->head->s->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(type_cmp(t_list->head->next->s, K_type) == 0);
    assert(t_list->count == 2);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(K_type);
    list_Pair_Sectoff_Type_dtor(t_list);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_types_load()
{
    assert(SectionTypes_LoadEmptySection_LoadNothing() == OK);
    assert(SectionTypes_LoadTypeWithoutAttributes_LoadType() == OK);
    assert(SectionTypes_LoadTypeWithAllAttributes_LoadType() == OK);
}