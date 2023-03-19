#include <assert.h>

#include "section/extents.h"
#include "section/extents_p.h"

static const char *test_file_name = "test.bin";

/*
1) Посмотреть обновление граничного элементы
2) Посмотреть обновление элемента внутри равного размера и хватает места
3) Посмотреть обновление элемента внутри размера меньше и хватает места
4) Посмотреть обновление элемента внутри размера больше и хватает места
5) Посмотреть обновление элемента когда не хватает места вставить
*/

static Status SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdate()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 500, 200, 300, 222);

    Fileoff save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), entity, &save_json_addr));

    String *str = string_new();
    string_ctor(str, "more_than_sixteen_elements_string", 33);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 500, 200, 300, 223);

    sect_ext_update(extents, save_json_addr, updated_json->key, json_val_ptr(updated_json), json_val_size(updated_json), updated_entity);

    Json *r_json = json_new();
    Entity *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity) - sizeof(Entity) - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(updated_json->key));
    assert(r_entity->key_size == string_get_size(updated_json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val));
    assert(r_entity->val_size == string_get_size(updated_json->value.string_val));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 200);
    assert(r_entity->fam_addr.son_ptr == 300);
    assert(r_entity->type == TYPE_STRING);
    assert(r_entity->type_ptr == 223);

    assert(string_cmp(r_json->key, updated_json->key) == 0);
    assert(string_cmp(r_json->value.string_val, updated_json->value.string_val) == 0);
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

static Status SectionExtents_UpdateInsideElementWithEqualSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0, 223);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0, 223);

    Fileoff first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json->key, json_val_ptr(first_json), json_val_size(first_json), f_entity, &first_json_addr));
    Fileoff second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json->key, json_val_ptr(second_json), json_val_size(second_json), s_entity, &second_json_addr));

    JSON_VALUE_INIT(TYPE_BOOL, updated_json, "value", true);
    ENTITY_INIT(updated_entity, updated_json, 300, 400, 500, 225);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json->key, json_val_ptr(updated_json), json_val_size(updated_json), updated_entity));

    Json *r_json = json_new();
    Entity *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity) - 2 * sizeof(Entity) - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(updated_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity) + 2 * sizeof(Entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - entity_rec_sz(s_entity) - entity_rec_sz(updated_entity));

    assert(r_entity->key_ptr == SECTION_SIZE - updated_entity->key_size);
    assert(r_entity->key_size == updated_entity->key_size);
    assert(r_entity->val_ptr == SECTION_SIZE - entity_rec_sz(updated_entity));
    assert(r_entity->val_size == sizeof(updated_json->value));
    assert(r_entity->fam_addr.dad_ptr == updated_entity->fam_addr.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == updated_entity->fam_addr.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == updated_entity->fam_addr.son_ptr);
    assert(r_entity->type == TYPE_BOOL);
    assert(r_entity->type_ptr == 225);

    assert(string_cmp(r_json->key, updated_json->key) == 0);
    assert(r_json->value.bool_val == updated_json->value.bool_val);
    assert(r_json->type == TYPE_BOOL);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);
    json_dtor(r_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(r_entity);
    entity_dtor(updated_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_UpdateInsideElementWithLessSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0, 220);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0, 225);

    Fileoff first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json->key, json_val_ptr(first_json), json_val_size(first_json), f_entity, &first_json_addr));
    Fileoff second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json->key, json_val_ptr(second_json), json_val_size(second_json), s_entity, &second_json_addr));

    String *str = string_new();
    string_ctor(str, "kek", 3);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 0, 0, 0, 300);

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, updated_json->key, json_val_ptr(updated_json), json_val_size(updated_json), updated_entity));

    Json *r_json = json_new();
    Entity *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_entity, r_json));

    assert(extents->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - entity_ph_size(f_entity) - entity_ph_size(s_entity));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(Sect_ext_entity) + 2 * sizeof(Entity));
    /*
    |     8 bits      |     8 bits      |          8 bits           |     8 bits       |
    | second_json_val | second_json_key |       first_json_val      |  first_json_key  |
    ------------------------------------------------------------------------------------
    |     8 bits      |      8 bits     | 5 bits |      3 bits      |     8 bits       |
    | second_json_val | second_json_key | empty  | updated_json_val | updated_json_key |
    */
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - entity_rec_sz(f_entity) - entity_rec_sz(s_entity));

    assert(r_entity->key_ptr == SECTION_SIZE - updated_entity->key_size);
    assert(r_entity->key_size == updated_entity->key_size);
    assert(r_entity->val_ptr == SECTION_SIZE - entity_rec_sz(updated_entity));
    assert(r_entity->val_size == updated_entity->val_size);
    assert(r_entity->fam_addr.dad_ptr == updated_entity->fam_addr.dad_ptr);
    assert(r_entity->fam_addr.bro_ptr == updated_entity->fam_addr.bro_ptr);
    assert(r_entity->fam_addr.son_ptr == updated_entity->fam_addr.son_ptr);
    assert(r_entity->type == TYPE_STRING);

    assert(string_cmp(r_json->key, updated_json->key) == 0);
    assert(string_cmp(r_json->value.string_val, updated_json->value.string_val) == 0);
    assert(r_json->type == TYPE_STRING);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);
    json_dtor(r_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(r_entity);
    entity_dtor(updated_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_UpdateInsideElementWithGreaterSizeWithEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_FLOAT, first_json, "float", 5.5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0, 225);

    JSON_VALUE_INIT(TYPE_INT32, second_json, "int", 5);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0, 229);

    String *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, third_json, "string", str);
    ENTITY_INIT(th_entity, third_json, 500, 0, 0, 300);

    Fileoff first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json->key, json_val_ptr(first_json), json_val_size(first_json), f_entity, &first_json_addr));
    Fileoff second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json->key, json_val_ptr(second_json), json_val_size(second_json), s_entity, &second_json_addr));

    DO_OR_FAIL(sect_ext_update(extents, first_json_addr, third_json->key, json_val_ptr(third_json), json_val_size(third_json), th_entity));

    Json *r_f_json = json_new();
    Entity *r_f_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, first_json_addr, r_f_entity, r_f_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(Sect_ext_entity) - 2 * sizeof(Entity) - string_get_size(first_json->key) - string_get_size(second_json->key) - string_get_size(third_json->key) - sizeof(first_json->value) - sizeof(second_json->value) - string_get_size(third_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(Sect_ext_entity) + 2 * sizeof(Entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key) - string_get_size(second_json->key) - string_get_size(third_json->key) - sizeof(first_json->value) - sizeof(second_json->value) - string_get_size(third_json->value.string_val));

    assert(r_f_entity->key_ptr == SECTION_SIZE - entity_rec_sz(f_entity) - entity_rec_sz(s_entity) - string_get_size(third_json->key));
    assert(r_f_entity->key_size == string_get_size(third_json->key));
    assert(r_f_entity->val_ptr == r_f_entity->key_ptr - string_get_size(third_json->value.string_val));
    assert(r_f_entity->val_size == string_get_size(third_json->value.string_val));
    assert(r_f_entity->fam_addr.dad_ptr == th_entity->fam_addr.dad_ptr);
    assert(r_f_entity->fam_addr.bro_ptr == th_entity->fam_addr.bro_ptr);
    assert(r_f_entity->fam_addr.son_ptr == th_entity->fam_addr.son_ptr);
    assert(r_f_entity->type == TYPE_STRING);
    assert(r_f_entity->type_ptr == th_entity->type_ptr);

    assert(string_cmp(r_f_json->key, third_json->key) == 0);
    assert(string_cmp(r_f_json->value.string_val, third_json->value.string_val) == 0);
    assert(r_f_json->type == TYPE_STRING);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(third_json);
    json_dtor(r_f_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(th_entity);
    entity_dtor(r_f_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_UpdateInsideElementWithGreaterSizeWithNotEnoughSpace_Update()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0, 200);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, first_json, 500, 0, 0, 200);

    Fileoff first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json->key, json_val_ptr(first_json), json_val_size(first_json), f_entity, &first_json_addr));
    Fileoff second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json->key, json_val_ptr(second_json), json_val_size(second_json), s_entity, &second_json_addr));

    String *str = string_new();
    string_ctor(str, "example_word", 12);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);
    ENTITY_INIT(updated_entity, updated_json, 500, 0, 0, 300);

    extents->header.free_space = 5;

    Status update_status = sect_ext_update(extents, first_json_addr, updated_json->key, json_val_ptr(updated_json), json_val_size(updated_json), updated_entity);

    json_dtor(first_json);
    json_dtor(second_json);
    json_dtor(updated_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(updated_entity);

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
