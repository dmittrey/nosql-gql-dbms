#include <assert.h>
#include <string.h>

#include "memory/section/types.h"
#include "memory/type/type.h"

#include "physical/section/types.h"
#include "physical/type/type.h"

static const char *test_file_name = "test.bin";

/*
1) Скачивание пустой секции
2) Скачивание типа без атрибутов
3) Скачивание типа со всеми атрибутами
*/

status_t SectionTypes_LoadEmptySection_LoadNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    list_type_t *t_list = list_type_t_new();

    sect_type_load(types, t_list);

    assert(t_list->head == NULL);
    assert(t_list->tail == NULL);
    assert(t_list->count == 0);

    list_type_t_dtor(t_list);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_LoadTypeWithoutAttributes_LoadType()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    type_t *wr_type = type_new();
    STR_INIT(type_name, "V");
    list_attr_t *wr_attr_list = list_attr_t_new();

    type_ctor(wr_type, type_name, wr_attr_list);
    sect_type_write(types, wr_type);

    list_type_t *t_list = list_type_t_new();

    sect_type_load(types, t_list);

    assert(string_cmp(t_list->head->name, wr_type->name) == 0);
    assert(t_list->head->attr_list->count == 0);
    assert(t_list->count == 1);

    type_dtor(wr_type);

    list_type_t_dtor(t_list);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_LoadTypeWithAllAttributes_LoadType()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    type_t *wr_type = type_new();
    STR_INIT(type_name, "V");
    list_attr_t *wr_attr_list = list_attr_t_new();

    attr_t *bool_attr = attr_new();
    STR_INIT(bool_attr_name, "attr");
    attr_ctor(bool_attr, bool_attr_name, TYPE_BOOL);
    list_attr_t_add(wr_attr_list, bool_attr);

    attr_t *int_attr = attr_new();
    STR_INIT(int_attr_name, "attr");
    attr_ctor(int_attr, int_attr_name, TYPE_INT32);
    list_attr_t_add(wr_attr_list, int_attr);

    attr_t *str_attr = attr_new();
    STR_INIT(str_attr_name, "attr");
    attr_ctor(str_attr, str_attr_name, TYPE_STRING);
    list_attr_t_add(wr_attr_list, str_attr);

    attr_t *float_attr = attr_new();
    STR_INIT(float_attr_name, "attr");
    attr_ctor(float_attr, float_attr_name, TYPE_FLOAT);
    list_attr_t_add(wr_attr_list, float_attr);

    type_ctor(wr_type, type_name, wr_attr_list);
    sect_type_write(types, wr_type);

    list_type_t *t_list = list_type_t_new();

    sect_type_load(types, t_list);

    assert(t_list->count == 1);
    assert(t_list->head == t_list->tail);
    assert(string_cmp(t_list->head->name, wr_type->name) == 0);
    assert(t_list->head->next == NULL);

    assert(t_list->head->attr_list->count == 4);
    assert(attr_cmp(t_list->head->attr_list->head, bool_attr) == 0);
    list_attr_t_del_fst(t_list->head->attr_list);
    assert(attr_cmp(t_list->head->attr_list->head, int_attr) == 0);
    list_attr_t_del_fst(t_list->head->attr_list);
    assert(attr_cmp(t_list->head->attr_list->head, str_attr) == 0);
    list_attr_t_del_fst(t_list->head->attr_list);
    assert(attr_cmp(t_list->head->attr_list->head, float_attr) == 0);
    list_attr_t_del_fst(t_list->head->attr_list);

    type_dtor(wr_type);

    list_type_t_dtor(t_list);

    sect_type_dtor(types);
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