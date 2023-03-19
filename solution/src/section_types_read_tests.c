#include <assert.h>

#include "section/types.h"
#include "section/types_p.h"

static const char *test_file_name = "test.bin";

/*
1) Чтение типа без атрибутов
2) Чтение типа с атрибутами
*/

Status SectionTypes_ReadTypeWithoutAttributes_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(wr_type, "V");

    Sectoff wrt_adr;
    sect_types_write(types, wr_type, &wrt_adr);

    Type *t = type_new();
    Type_entity *t_ent = type_entity_new();
    size_t ph_sz = 0;
    sect_types_read(types, wrt_adr, t, t_ent, &ph_sz);

    assert(t_ent->attr_cnt == wr_type->attr_list->count);
    assert(t_ent->name_size == wr_type->name->cnt);
    assert(t_ent->name_ptr == SECTION_SIZE - wr_type->name->cnt);

    assert(string_cmp(t->name, wr_type->name) == 0);
    assert(t->attr_list->count == 0);

    sect_types_dtor(types);
    type_dtor(t);
    type_dtor(wr_type);
    type_entity_dtor(t_ent);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_ReadTypeWithAttributes_Successful()
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

    Sectoff wrt_adr;
    sect_types_write(types, V_type, &wrt_adr);

    Type *t = type_new();
    Type_entity *t_ent = type_entity_new();
    size_t ph_sz = 0;
    sect_types_read(types, wrt_adr, t, t_ent, &ph_sz);

    assert(t_ent->attr_cnt == V_type->attr_list->count);
    assert(t_ent->name_size == V_type->name->cnt);
    assert(t_ent->name_ptr == SECTION_SIZE - V_type->name->cnt);

    assert(string_cmp(t->name, V_type->name) == 0);
    assert(t->attr_list->count == V_type->attr_list->count);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(t);
    type_entity_dtor(t_ent);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_types_read()
{
    assert(SectionTypes_ReadTypeWithoutAttributes_Successful() == OK);
    assert(SectionTypes_ReadTypeWithAttributes_Successful() == OK);
}