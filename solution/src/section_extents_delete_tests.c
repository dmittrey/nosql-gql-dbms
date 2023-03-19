#include <assert.h>

#include "section/extents.h"
#include "section/extents_p.h"

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

static Status SectionExtents_DeleteOnBorderType_ShiftPtrsAndClearRecAndItm(const Json *const json)
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    ENTITY_INIT(ent, json, 500, 200, 300, 222);

    Sectoff save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), ent, &save_json_sectoff));

    Entity *del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, del_entity));

    assert(entity_cmp(ent, del_entity) == 0);

    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity));

    Entity *o_entity = entity_new();
    Entity *temp_zero_entity = entity_new();
    sect_ext_rd_itm(extents, save_json_sectoff, o_entity);
    assert(entity_cmp(o_entity, temp_zero_entity) == 0);

    void *o_rec = my_malloc_array(char, entity_rec_sz(del_entity));
    void *temp_zero = calloc(1, entity_rec_sz(del_entity));
    sect_head_read((Sect_head *)extents, del_entity->key_ptr - del_entity->val_size, entity_rec_sz(del_entity), o_rec);
    assert(memcmp(o_rec, temp_zero, entity_rec_sz(del_entity)) == 0);

    free(temp_zero);
    free(o_rec);

    entity_dtor(ent);
    entity_dtor(del_entity);
    entity_dtor(o_entity);
    entity_dtor(temp_zero_entity);

    sect_ext_dtor(extents);

    return OK;
}

static Status SectionExtents_DeleteInnerType_ShiftPtrsAndClearRecAndItm(const Json *const j)
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    ENTITY_INIT(ent, j, 0, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, brdr_json, "brdr_json", VAL);
    ENTITY_INIT(brdr_entity, brdr_json, 0, 0, 0, 0);

    Sectoff save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, j->key, json_val_ptr(j), json_val_size(j), ent, &save_json_sectoff));
    Sectoff save_brdr_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, brdr_json->key, json_val_ptr(brdr_json), json_val_size(brdr_json), brdr_entity, &save_brdr_json_sectoff));

    Entity *del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, del_entity));

    assert(entity_cmp(ent, del_entity) == 0);

    Entity *o_entity = entity_new();
    Entity *temp_zero_entity = entity_new();
    sect_ext_rd_itm(extents, save_json_sectoff, o_entity);
    assert(entity_cmp(o_entity, temp_zero_entity) == 0);

    void *o_rec = my_malloc_array(char, entity_rec_sz(del_entity));
    void *temp_zero = memset(my_malloc_array(char, entity_rec_sz(del_entity)), 0, entity_rec_sz(del_entity));
    sect_head_read((Sect_head *)extents, del_entity->key_ptr - del_entity->val_size, entity_rec_sz(del_entity), o_rec);
    assert(memcmp(o_rec, temp_zero, entity_rec_sz(del_entity)) == 0);

    free(o_rec);
    free(temp_zero);

    json_dtor(brdr_json);

    entity_dtor(ent);
    entity_dtor(del_entity);
    entity_dtor(brdr_entity);
    entity_dtor(o_entity);
    entity_dtor(temp_zero_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_DeleteOnBorderElementWithLeftGap_ReduceGapsAndDelete()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    JSON_VALUE_INIT(TYPE_OBJECT, j, "json", NULL);
    ENTITY_INIT(ent, j, 500, 200, 300, 222);

    JSON_VALUE_INIT(TYPE_OBJECT, brdr_json, "brdr_json", NULL);
    ENTITY_INIT(brdr_entity, brdr_json, 0, 0, 0, 222);

    Sectoff save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, j->key, json_val_ptr(j), json_val_size(j), ent, &save_json_sectoff));
    Sectoff save_brdr_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, brdr_json->key, json_val_ptr(brdr_json), json_val_size(brdr_json), brdr_entity, &save_brdr_json_sectoff));

    DO_OR_FAIL(sect_ext_delete(extents, save_json_sectoff, NULL));
    DO_OR_FAIL(sect_ext_delete(extents, save_brdr_json_sectoff, NULL));

    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity));

    json_dtor(j);
    json_dtor(brdr_json);

    entity_dtor(ent);
    entity_dtor(brdr_entity);

    sect_ext_dtor(extents);

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