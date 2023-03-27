#include "tests/test.h"

#include "section/extents.h"
#include "section/extents_p.h"

static const char *test_file_name = "test.bin";

static Status SectionExtents_ReadTypedJson_Successful(Json *json)
{
    int START_ADR = 555;
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, START_ADR, file);

    Sectoff save_json_addr;
    ENTITY_INIT(entity, json, 500, 200, 300, 222);
    DO_OR_FAIL(sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), entity, &save_json_addr));

    Json *r_json = json_new();
    Entity *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    // Test entity read
    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(r_entity->key_size == string_get_size(json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - json_val_size(json));
    assert(r_entity->val_size == json_val_size(json));
    assert(r_entity->fam_addr.dad_ptr == 500);
    assert(r_entity->fam_addr.bro_ptr == 200);
    assert(r_entity->fam_addr.son_ptr == 300);
    assert(r_entity->type == json->type);
    assert(r_entity->init_rec_sz == entity->init_rec_sz);
    assert(r_entity->type_ptr == 222);

    // Test json read
    assert(string_cmp(r_json->key, json->key) == 0);
    assert(r_json->type == json->type);
    if (r_json->type == TYPE_STRING)
    {
        assert(string_cmp(r_json->value.string_val, json->value.string_val) == 0);
    } else {
        assert(memcmp(json_val_ptr(r_json), json_val_ptr(json), json_val_size(r_json)) == 0);
    }

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
    JSON_VALUE_INIT(TYPE_INT32, int32_j, "int32", 5);
    assert(SectionExtents_ReadTypedJson_Successful(int32_j) == OK);
    JSON_VALUE_INIT(TYPE_FLOAT, float_j, "float", 5.5);
    assert(SectionExtents_ReadTypedJson_Successful(float_j) == OK);
    JSON_VALUE_INIT(TYPE_BOOL, bool_j, "bool", true);
    assert(SectionExtents_ReadTypedJson_Successful(bool_j) == OK);
    STR_INIT(str, "kekeke");
    JSON_VALUE_INIT(TYPE_STRING, string_j, "string", str);
    assert(SectionExtents_ReadTypedJson_Successful(string_j) == OK);
    JSON_VALUE_INIT(TYPE_OBJECT, obj_j, "object", NULL);
    assert(SectionExtents_ReadTypedJson_Successful(obj_j) == OK);
}
