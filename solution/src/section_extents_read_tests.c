#include <assert.h>

#include "physical/json/entity.h"

#include "physical/section/header.h"

#include "memory/section/extents_p.h"
#include "memory/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_ReadInt32Json_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(r_entity->val_size == sizeof(json->value));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 0);
    assert(r_entity->fam_addr.son_ptr == 0);
    assert(r_entity->type == TYPE_INT32);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.int32_val == json->value.int32_val);
    assert(r_json->type == TYPE_INT32);

    json_dtor(json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_ReadFloatJson_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(r_entity->val_size == sizeof(json->value));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 0);
    assert(r_entity->fam_addr.son_ptr == 0);
    assert(r_entity->type == TYPE_FLOAT);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.float_val == json->value.float_val);
    assert(r_json->type == TYPE_FLOAT);

    json_dtor(json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_ReadBoolJson_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(r_entity->val_size == sizeof(json->value));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 0);
    assert(r_entity->fam_addr.son_ptr == 0);
    assert(r_entity->type == TYPE_BOOL);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.bool_val == json->value.bool_val);
    assert(r_json->type == TYPE_BOOL);

    json_dtor(json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_ReadStringJson_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    string_t *str = string_new();
    string_ctor(str, "test_value", 10);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - string_get_size(json->value.string_val));
    assert(r_entity->val_size == string_get_size(json->value.string_val));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 0);
    assert(r_entity->fam_addr.son_ptr == 0);
    assert(r_entity->type == TYPE_STRING);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.string_val->cnt == json->value.string_val->cnt);
    assert(strncmp(r_json->value.string_val->val, json->value.string_val->val, r_json->value.string_val->cnt) == 0);
    assert(r_json->type == TYPE_STRING);

    json_dtor(json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_ReadObjectJson_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_OBJECT, json, "value", NULL);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->val_size == 0);
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 0);
    assert(r_entity->fam_addr.son_ptr == 0);
    assert(r_entity->type == TYPE_OBJECT);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->type == TYPE_OBJECT);

    json_dtor(json);
    json_dtor(r_json);

    entity_dtor(entity);
    entity_dtor(r_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_read()
{
    assert(SectionExtents_ReadInt32Json_Successful() == OK);
    assert(SectionExtents_ReadFloatJson_Successful() == OK);
    assert(SectionExtents_ReadBoolJson_Successful() == OK);
    assert(SectionExtents_ReadStringJson_Successful() == OK);
    assert(SectionExtents_ReadObjectJson_Successful() == OK);
}
