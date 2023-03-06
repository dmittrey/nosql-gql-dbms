#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"
/*
1) Посмотреть обновление граничного элементы
2) Посмотреть обновление элемента внутри равного размера и хватает места
3) Посмотреть обновление элемента внутри размера меньше и хватает места
4) Посмотреть обновление элемента внутри размера больше и хватает места
5) Посмотреть обновление элемента когда не хватает места вставить
*/

static const char *test_file_name = "test.bin";

static status_t SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdate()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 500, 200, 300);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    string_t *str = string_new();
    string_ctor(str, "more_than_sixteen_elements_string", 33);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 500, 200, 300);

    DO_OR_FAIL(sect_ext_update(extents, save_json_addr, updated_json, updated_entity));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == r_entity->val_ptr);

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(updated_json->key));
    assert(r_entity->key_size == string_get_size(updated_json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val));
    assert(r_entity->val_size == string_get_size(updated_json->value.string_val));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 200);
    assert(r_entity->fam_addr.son_ptr == 300);
    assert(r_entity->type == TYPE_STRING);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.string_val->cnt == updated_json->value.string_val->cnt);
    assert(strncmp(r_json->value.string_val->val, updated_json->value.string_val->val, r_json->value.string_val->cnt) == 0);
    assert(r_json->type == TYPE_STRING);

    json_dtor(json);
    json_dtor(updated_json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithEqualSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0);

    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, f_entity, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, s_entity, &second_json_addr));

    JSON_VALUE_INIT(TYPE_BOOL, updated_json, "value", true);
    ENTITY_INIT(updated_entity, updated_json, 500, 0, 0);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json, updated_entity));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(updated_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(s_entity) - entity_rec_size(updated_entity));

    assert(r_entity->key_ptr == SECTION_SIZE - updated_entity->key_size);
    assert(r_entity->key_size == updated_entity->key_size);
    assert(r_entity->val_ptr == SECTION_SIZE - entity_rec_size(updated_entity));
    assert(r_entity->val_size == sizeof(updated_json->value));
    assert(r_entity->fam_addr.dad_ptr == f_entity->fam_addr.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == f_entity->fam_addr.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == f_entity->fam_addr.son_ptr);
    assert(r_entity->type == TYPE_BOOL);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.bool_val == updated_json->value.bool_val);
    assert(r_json->type == TYPE_BOOL);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);
    json_dtor(r_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithLessSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0);

    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, f_entity, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, s_entity, &second_json_addr));

    string_t *str = string_new();
    string_ctor(str, "kek", 3);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 0, 0, 0);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json, updated_entity));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(f_entity) - entity_ph_size(s_entity));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    /*
    |     8 bits      |     8 bits      |          8 bits           |     8 bits       |
    | second_json_val | second_json_key |       first_json_val      |  first_json_key  |
    ------------------------------------------------------------------------------------
    |     8 bits      |      8 bits     | 5 bits |      3 bits      |     8 bits       |
    | second_json_val | second_json_key | empty  | updated_json_val | updated_json_key |
    */
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(f_entity) - entity_rec_size(s_entity));

    assert(r_entity->key_ptr == SECTION_SIZE - updated_entity->key_size);
    assert(r_entity->key_size == updated_entity->key_size);
    assert(r_entity->val_ptr == SECTION_SIZE - entity_rec_size(updated_entity));
    assert(r_entity->val_size == updated_entity->val_size);
    assert(r_entity->fam_addr.dad_ptr == updated_entity->fam_addr.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == updated_entity->fam_addr.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == updated_entity->fam_addr.son_ptr);
    assert(r_entity->type == TYPE_STRING);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.string_val->cnt == updated_json->value.string_val->cnt);
    assert(strncmp(r_json->value.string_val->val, updated_json->value.string_val->val, r_json->value.string_val->cnt) == 0);
    assert(r_json->type == TYPE_STRING);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);
    json_dtor(r_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithGreaterSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    string_t *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, first_json, "value", str);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 5);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, third_json, "value", 6);
    ENTITY_INIT(th_entity, third_json, 500, 0, 0);

    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, f_entity, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, s_entity, &second_json_addr));

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, third_json, th_entity));
    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, first_json, f_entity));

    json_t *r_f_json = json_new();
    entity_t *r_f_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_f_entity, r_f_json));

    json_t *r_s_json = json_new();
    entity_t *r_s_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, second_json_addr, r_s_entity, r_s_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - sizeof(second_json->value) - string_get_size(first_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key) - string_get_size(second_json->key) - sizeof(second_json->value) - string_get_size(first_json->value.string_val));

    assert(r_f_entity->key_ptr == SECTION_SIZE - string_get_size(first_json->key));
    assert(r_f_entity->key_size == string_get_size(first_json->key));
    assert(r_f_entity->val_ptr == r_f_entity->key_ptr - string_get_size(first_json->value.string_val));
    assert(r_f_entity->val_size == string_get_size(first_json->value.string_val));
    assert(r_f_entity->fam_addr.dad_ptr == f_entity->fam_addr.dad_ptr);
    assert(r_f_entity->fam_addr.bro_ptr == f_entity->fam_addr.bro_ptr);
    assert(r_f_entity->fam_addr.son_ptr == f_entity->fam_addr.son_ptr);
    assert(r_f_entity->type == TYPE_STRING);

    assert(r_f_json->key->cnt == first_json->key->cnt);
    assert(strncmp(r_f_json->key->val, first_json->key->val, r_f_json->key->cnt) == 0);
    assert(r_f_json->value.string_val->cnt == first_json->value.string_val->cnt);
    assert(strncmp(r_f_json->value.string_val->val, first_json->value.string_val->val, r_f_json->value.string_val->cnt) == 0);
    assert(r_f_json->type == TYPE_STRING);

    assert(r_s_entity->key_ptr == r_f_entity->val_ptr - string_get_size(second_json->key));
    assert(r_s_entity->key_size == string_get_size(second_json->key));
    assert(r_s_entity->val_ptr == r_s_entity->key_ptr - sizeof(second_json->value));
    assert(r_s_entity->val_size == sizeof(second_json->value));
    assert(r_s_entity->fam_addr.dad_ptr == s_entity->fam_addr.dad_ptr);
    assert(r_s_entity->fam_addr.bro_ptr == s_entity->fam_addr.bro_ptr);
    assert(r_s_entity->fam_addr.son_ptr == s_entity->fam_addr.son_ptr);
    assert(r_s_entity->type == TYPE_INT32);

    assert(r_s_json->key->cnt == second_json->key->cnt);
    assert(strncmp(r_s_json->key->val, second_json->key->val, r_s_json->key->cnt) == 0);
    assert(r_s_json->value.int32_val == second_json->value.int32_val);
    assert(r_s_json->type == TYPE_INT32);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(third_json);
    json_dtor(r_f_json);
    json_dtor(r_s_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(th_entity);
    entity_dtor(r_f_entity);
    entity_dtor(r_s_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithGreaterSizeWithNotEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, first_json, 500, 0, 0);

    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, f_entity, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, s_entity, &second_json_addr));

    string_t *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 500, 0, 0);

    status_t update_status = sect_ext_update(extents, first_json_addr, updated_json, updated_entity);

    json_t *r_f_json = json_new();
    entity_t *r_f_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_f_entity, r_f_json));

    json_t *r_s_json = json_new();
    entity_t *r_s_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, second_json_addr, r_s_entity, r_s_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(second_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(first_json->value));

    assert(r_f_entity->key_ptr == SECTION_SIZE - string_get_size(first_json->key));
    assert(r_f_entity->key_size == string_get_size(first_json->key));
    assert(r_f_entity->val_ptr == SECTION_SIZE - string_get_size(first_json->key) - sizeof(first_json->value));
    assert(r_f_entity->val_size == sizeof(first_json->value));
    assert(r_f_entity->fam_addr.dad_ptr == f_entity->fam_addr.dad_ptr);
    assert(r_f_entity->fam_addr.bro_ptr == f_entity->fam_addr.bro_ptr);
    assert(r_f_entity->fam_addr.son_ptr == f_entity->fam_addr.son_ptr);
    assert(r_f_entity->type == TYPE_INT32);

    assert(r_f_json->key->cnt == first_json->key->cnt);
    assert(strncmp(r_f_json->key->val, first_json->key->val, r_f_json->key->cnt) == 0);
    assert(r_f_json->value.int32_val == first_json->value.int32_val);

    assert(r_s_entity->key_ptr == r_f_entity->val_ptr - string_get_size(second_json->key));
    assert(r_s_entity->key_size == string_get_size(second_json->key));
    assert(r_s_entity->val_ptr == r_f_entity->val_ptr - string_get_size(second_json->key) - sizeof(first_json->value));
    assert(r_s_entity->val_size == sizeof(second_json->value));
    assert(r_s_entity->fam_addr.dad_ptr == s_entity->fam_addr.dad_ptr);
    assert(r_s_entity->fam_addr.bro_ptr == s_entity->fam_addr.bro_ptr);
    assert(r_s_entity->fam_addr.son_ptr == s_entity->fam_addr.son_ptr);
    assert(r_s_entity->type == TYPE_INT32);

    assert(r_s_json->key->cnt == second_json->key->cnt);
    assert(strncmp(r_s_json->key->val, second_json->key->val, r_s_json->key->cnt) == 0);
    assert(r_s_json->value.int32_val == second_json->value.int32_val);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);
    json_dtor(r_f_json);
    json_dtor(r_s_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(r_f_entity);
    entity_dtor(r_s_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return update_status;
}

void test_extents_update()
{
    assert(SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdate() == OK);
    assert(SectionExtents_UpdateInsideElementWithEqualSizeWithEnoughSpace_Update() == OK);
    assert(SectionExtents_UpdateInsideElementWithLessSizeWithEnoughSpace_Update() == OK);
    assert(SectionExtents_UpdateInsideElementWithGreaterSizeWithEnoughSpace_Update() == OK);
    assert(SectionExtents_UpdateInsideElementWithGreaterSizeWithNotEnoughSpace_Update() == FAILED);
}
