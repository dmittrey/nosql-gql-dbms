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
5) посмотреть обновление элемента когда не хватает места вставить
*/

static const char *test_file_name = "test.bin";

static status_t SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdate()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t save_json_addr;
    tplgy_addr tplgy_addrs = (tplgy_addr){.dad_ptr = 500, .bro_ptr = 200, .son_ptr = 300};
    DO_OR_FAIL(sect_ext_write(extents, json, tplgy_addrs, &save_json_addr));

    string_t *str = string_new();
    string_ctor(str, "more_than_sixteen_elements_string", 33);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);

    DO_OR_FAIL(sect_ext_update(extents, save_json_addr, updated_json));

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

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithEqualSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);

    tplgy_addr tplgy_addrs = (tplgy_addr){.dad_ptr = 500, .bro_ptr = 200, .son_ptr = 300};
    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, tplgy_addrs, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, tplgy_addrs, &second_json_addr));

    JSON_VALUE_INIT(TYPE_BOOL, updated_json, "value", true);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(updated_json->key) - string_get_size(second_json->key) - 2 * sizeof(updated_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == r_entity->val_ptr - string_get_size(second_json->key) - sizeof(second_json->value));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(updated_json->key));
    assert(r_entity->key_size == string_get_size(updated_json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(updated_json->key) - sizeof(updated_json->value));
    assert(r_entity->val_size == sizeof(updated_json->value));
    assert(r_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_entity->type == TYPE_BOOL);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.bool_val == updated_json->value.bool_val);
    assert(r_json->type == TYPE_BOOL);

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithLessSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);

    tplgy_addr tplgy_addrs = (tplgy_addr){.dad_ptr = 500, .bro_ptr = 200, .son_ptr = 300};
    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, tplgy_addrs, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, tplgy_addrs, &second_json_addr));

    string_t *str = string_new();
    string_ctor(str, "kek", 3);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(updated_json->key) - string_get_size(second_json->key) - string_get_size(updated_json->value.string_val) - sizeof(second_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    /*
    |     8 bits      |     8 bits      |          8 bits           |     8 bits       |
    | second_json_val | second_json_key |       first_json_val      |  first_json_key  |
    ------------------------------------------------------------------------------------
    |     8 bits      |      8 bits     | 5 bits |      3 bits      |     8 bits       |
    | second_json_val | second_json_key | empty  | updated_json_val | updated_json_key |
    */
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key) - sizeof(first_json->value) - string_get_size(second_json->key) - sizeof(second_json->value));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(updated_json->key));
    assert(r_entity->key_size == string_get_size(updated_json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val));
    assert(r_entity->val_size == string_get_size(updated_json->value.string_val));
    assert(r_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_entity->type == TYPE_STRING);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.string_val->cnt == updated_json->value.string_val->cnt);
    assert(strncmp(r_json->value.string_val->val, updated_json->value.string_val->val, r_json->value.string_val->cnt) == 0);
    assert(r_json->type == TYPE_STRING);

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithGreaterSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);

    string_t *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, second_json, "value", str);

    JSON_VALUE_INIT(TYPE_INT32, third_json, "value", 6);

    tplgy_addr tplgy_addrs = (tplgy_addr){.dad_ptr = 500, .bro_ptr = 200, .son_ptr = 300};
    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, tplgy_addrs, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, tplgy_addrs, &second_json_addr));

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, third_json));
    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, second_json));

    json_t *r_f_json = json_new();
    entity_t *r_f_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_f_entity, r_f_json));

    json_t *r_s_json = json_new();
    entity_t *r_s_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, second_json_addr, r_s_entity, r_s_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - sizeof(first_json->value) - string_get_size(second_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key)  - string_get_size(second_json->key) - sizeof(first_json->value) - string_get_size(second_json->value.string_val));

    assert(r_f_entity->key_ptr == SECTION_SIZE - string_get_size(first_json->key));
    assert(r_f_entity->key_size == string_get_size(first_json->key));
    assert(r_f_entity->val_ptr == r_f_entity->key_ptr - sizeof(first_json->value));
    assert(r_f_entity->val_size == sizeof(first_json->value));
    assert(r_f_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_f_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_f_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_f_entity->type == TYPE_INT32);

    assert(r_f_json->key->cnt == first_json->key->cnt);
    assert(strncmp(r_f_json->key->val, first_json->key->val, r_f_json->key->cnt) == 0);
    assert(r_f_json->value.int32_val == first_json->value.int32_val);

    assert(r_s_entity->key_ptr == r_f_entity->val_ptr - string_get_size(second_json->key));
    assert(r_s_entity->key_size == string_get_size(second_json->key));
    assert(r_s_entity->val_ptr == r_s_entity->key_ptr - string_get_size(second_json->value.string_val));
    assert(r_s_entity->val_size == string_get_size(second_json->value.string_val));
    assert(r_s_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_s_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_s_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_s_entity->type == TYPE_STRING);

    assert(r_s_json->key->cnt == second_json->key->cnt);
    assert(strncmp(r_s_json->key->val, second_json->key->val, r_s_json->key->cnt) == 0);
    assert(r_s_json->value.string_val->cnt == second_json->value.string_val->cnt);
    assert(strncmp(r_s_json->value.string_val->val, second_json->value.string_val->val, r_s_json->value.string_val->cnt) == 0);
    assert(r_s_json->type == TYPE_STRING);

    return OK;
}

static status_t SectionExtents_UpdateInsideElementWithGreaterSizeWithNotEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);

    tplgy_addr tplgy_addrs = (tplgy_addr){.dad_ptr = 500, .bro_ptr = 200, .son_ptr = 300};
    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, tplgy_addrs, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, tplgy_addrs, &second_json_addr));

    string_t *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);

    status_t update_status = sect_ext_update(extents, first_json_addr, updated_json);

    json_t *r_f_json = json_new();
    entity_t *r_f_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_f_entity, r_f_json));

    json_t *r_s_json = json_new();
    entity_t *r_s_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, second_json_addr, r_s_entity, r_s_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(second_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key)  - string_get_size(second_json->key) - 2 * sizeof(first_json->value));

    assert(r_f_entity->key_ptr == SECTION_SIZE - string_get_size(first_json->key));
    assert(r_f_entity->key_size == string_get_size(first_json->key));
    assert(r_f_entity->val_ptr == SECTION_SIZE - string_get_size(first_json->key) - sizeof(first_json->value));
    assert(r_f_entity->val_size == sizeof(first_json->value));
    assert(r_f_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_f_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_f_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_f_entity->type == TYPE_INT32);

    assert(r_f_json->key->cnt == first_json->key->cnt);
    assert(strncmp(r_f_json->key->val, first_json->key->val, r_f_json->key->cnt) == 0);
    assert(r_f_json->value.int32_val == first_json->value.int32_val);

    assert(r_s_entity->key_ptr == r_f_entity->val_ptr - string_get_size(second_json->key));
    assert(r_s_entity->key_size == string_get_size(second_json->key));
    assert(r_s_entity->val_ptr == r_f_entity->val_ptr - string_get_size(second_json->key) - sizeof(first_json->value));
    assert(r_s_entity->val_size == sizeof(second_json->value));
    assert(r_s_entity->fam_addr.dad_ptr == tplgy_addrs.dad_ptr);
    assert(r_s_entity->fam_addr.bro_ptr == tplgy_addrs.bro_ptr);
    assert(r_s_entity->fam_addr.son_ptr == tplgy_addrs.son_ptr);
    assert(r_s_entity->type == TYPE_INT32);

    assert(r_s_json->key->cnt == second_json->key->cnt);
    assert(strncmp(r_s_json->key->val, second_json->key->val, r_s_json->key->cnt) == 0);
    assert(r_s_json->value.int32_val == second_json->value.int32_val);

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
