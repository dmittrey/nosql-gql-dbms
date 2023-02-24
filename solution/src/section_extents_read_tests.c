/*
По базе прочитать все 4 типа и object
*/
#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_ReadInt32Json_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t save_json_addr;

    DO_OR_FAIL(sect_ext_write(extents, json, 500, 0, 0, &save_json_addr));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(r_entity->val_size == sizeof(json->value));
    assert(r_entity->dad_ptr == 500);
    assert(r_entity->bro_ptr == 0);
    assert(r_entity->son_ptr == 0);
    assert(r_entity->type == TYPE_INT32);

    assert(r_json->key->cnt == json->key->cnt);
    assert(strncmp(r_json->key->val, json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.int32_val == json->value.int32_val);
    assert(r_json->type == TYPE_INT32);

    return OK;
}

static status_t SectionExtents_ReadFloatJson_Successful()
{
}

static status_t SectionExtents_ReadBoolJson_Successful()
{
}

static status_t SectionExtents_ReadStringJson_Successful()
{
}

static status_t SectionExtents_ReadObjectJson_Successful()
{
}

void test_extents_read()
{
    SectionExtents_ReadInt32Json_Successful();
}
