#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"
/*
1) Удаление TYPE_OBJECT на границе
2) Удаление TYPE_STRING на границе
3) Удаление TYPE_INT32 на границе
4) Удаление TYPE_FLOAT на границе
5) Удаление TYPE_BOOL на границе
6) Удаление TYPE_OBJECT внутри сущностей
7) Удаление TYPE_STRING внутри сущностей
8) Удаление TYPE_INT32 внутри сущностей
9) Удаление TYPE_FLOAT внутри сущностей
10) Удаление TYPE_BOOL внутри сущностей
11) Удалить элемент на границе и слева от него пусто
*/

static const char *test_file_name = "test.bin";

static status_t SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(const json_t * const json)
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    ENTITY_INIT(entity, json, 500, 200, 300);

    sectoff_t save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));

    entity_t *del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, del_entity));

    assert(entity_cmp(entity, del_entity) == 0);

    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));

    entity_t *o_entity = entity_new();
    entity_t *temp_zero_entity = entity_new();
    void *o_rec = my_malloc_array(char, entity_rec_size(del_entity));
    void *temp_zero = memset(my_malloc_array(char, entity_rec_size(del_entity)), 0, entity_rec_size(del_entity));
    sect_ext_rd_itm(extents, save_json_sectoff, o_entity);
    sect_ext_rd_rec(extents, del_entity->key_ptr, entity_rec_size(del_entity), o_rec);
    assert(entity_cmp(o_entity, temp_zero_entity) == 0);
    assert(memcmp(o_rec, temp_zero, entity_rec_size(del_entity)) == 0);

    return OK;
}

static status_t SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(const json_t * const json)
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    ENTITY_INIT(entity, json, 500, 200, 300);

    JSON_VALUE_INIT(TYPE_OBJECT, brdr_json, "brdr_json", VAL);
    ENTITY_INIT(brdr_entity, brdr_json, 0, 0, 0);

    sectoff_t save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));
    sectoff_t save_brdr_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, brdr_json, brdr_entity, &save_brdr_json_sectoff));

    entity_t *del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, del_entity));

    assert(entity_cmp(entity, del_entity) == 0);

    entity_t *o_entity = entity_new();
    entity_t *temp_zero_entity = entity_new();
    void *o_rec = my_malloc_array(char, entity_rec_size(del_entity));
    void *temp_zero = memset(my_malloc_array(char, entity_rec_size(del_entity)), 0, entity_rec_size(del_entity));
    sect_ext_rd_itm(extents, save_json_sectoff, o_entity);
    sect_ext_rd_rec(extents, del_entity->key_ptr - del_entity->val_size, entity_rec_size(del_entity), o_rec);
    assert(entity_cmp(o_entity, temp_zero_entity) == 0);
    assert(memcmp(o_rec, temp_zero, entity_rec_size(del_entity)) == 0);

    free(o_rec);
    free(temp_zero);

    entity_dtor(entity);
    entity_dtor(del_entity);
    entity_dtor(o_entity);
    entity_dtor(temp_zero_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_DeleteOnBorderElementWithLeftGap_ReduceGapsAndDelete()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    JSON_VALUE_INIT(TYPE_OBJECT, json, "json", NULL);
    ENTITY_INIT(entity, json, 500, 200, 300);

    JSON_VALUE_INIT(TYPE_OBJECT, brdr_json, "brdr_json", NULL);
    ENTITY_INIT(brdr_entity, brdr_json, 0, 0, 0);

    sectoff_t save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));
    sectoff_t save_brdr_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, brdr_json, brdr_entity, &save_brdr_json_sectoff));

    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, NULL));
    entity_t *del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, save_brdr_json_sectoff, NULL));

    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));

    return OK;
}

void test_extents_delete()
{
    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "json", NULL);
    STR_INIT(json_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, str_json, "json", json_str);
    JSON_VALUE_INIT(TYPE_INT32, int_json, "json", 5);
    JSON_VALUE_INIT(TYPE_FLOAT, float_json, "json", 5.5);
    JSON_VALUE_INIT(TYPE_BOOL, bool_json, "json", true);

    assert(SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(obj_json) == OK);
    assert(SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(str_json) == OK);
    assert(SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(int_json) == OK);
    assert(SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(float_json) == OK);
    assert(SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(bool_json) == OK);

    assert(SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(obj_json) == OK);
    assert(SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(str_json) == OK);
    assert(SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(int_json) == OK);
    assert(SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(float_json) == OK);
    assert(SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(bool_json) == OK);

    assert(SectionExtents_DeleteOnBorderElementWithLeftGap_ReduceGapsAndDelete() == OK);

    json_dtor(obj_json);
    json_dtor(str_json);
    json_dtor(int_json);
    json_dtor(float_json);
    json_dtor(bool_json);
}