#include <assert.h>
#include <string.h>

#include "section/section_extents.h"

#include "entity/json_value_entity.h"
#include "entity/section_extents_entity.h"

static const char *test_file_name = "test.bin";

static PerformStatus SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    assert(extents->header.free_space == (SECTION_SIZE - section_header_size()));
    assert(extents->header.next == 0); // Next section is undefined
    assert(extents->header.last_item_ptr == section_header_size());
    assert(extents->header.first_record_ptr == SECTION_SIZE);

    // section_extents_dtor(extents);   // No make sense
    free(extents); // Make sense
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_InvokeSync_InvokeHeaderSync()
{
    FILE *file = fopen(test_file_name, "w+");
    section_extents_t *extents = section_extents_new();

    extents->header.free_space = 5;
    extents->header.next = 6;
    extents->header.last_item_ptr = 7;
    extents->header.first_record_ptr = 8;
    extents->header.section_offset = 0;
    extents->header.filp = file;

    DO_OR_FAIL(section_extents_sync(extents));

    section_header_entity header;
    FREAD_OR_FAIL(&header, sizeof(section_header_entity), file);

    assert(header.free_space == 5);
    assert(header.next_ptr == 6);
    assert(header.last_item_ptr == 7);
    assert(header.first_record_ptr == 8);

    DO_OR_FAIL(section_extents_dtor(extents));
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteInt32JsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    PerformStatus write_status = section_extents_write(extents, json, parent_json_addr, 0, 0, &save_json_addr);
    assert(save_json_addr == section_header_size());

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity_t) - sizeof(json->value) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header.first_record_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header.last_item_ptr == section_header_size() + sizeof(json_value_entity_t));

    json_value_entity_t json_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_entity), save_json_addr, file);
    assert(json_entity.key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity.key_size == string_get_size(json->key));
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity.val_size == sizeof(json->value));
    assert(json_entity.dad_ptr == 500);
    assert(json_entity.bro_ptr == 0);
    assert(json_entity.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity.key_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity.key_size, json_entity.key_ptr, file);
    assert(strncmp(key_val, json->key.val, json->key.count) == 0);
    free(key_val);

    int32_t val_val;
    RANDOM_ACCESS_FREAD_OR_FAIL(&val_val, sizeof(int32_t), json_entity.val_ptr, file);
    assert(val_val == json->value.int32_val);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return write_status;
}

static PerformStatus SectionExtents_WriteFloatJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    PerformStatus write_status = section_extents_write(extents, json, parent_json_addr, 0, 0, &save_json_addr);
    assert(save_json_addr == section_header_size());

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity_t) - sizeof(json->value) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header.first_record_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header.last_item_ptr == section_header_size() + sizeof(json_value_entity_t));

    json_value_entity_t json_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_value_entity_t), save_json_addr, file);
    assert(json_entity.key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity.key_size == string_get_size(json->key));
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity.val_size == sizeof(json->value));
    assert(json_entity.dad_ptr == 500);
    assert(json_entity.bro_ptr == 0);
    assert(json_entity.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity.key_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity.key_size, json_entity.key_ptr, file);
    assert(strncmp(key_val, json->key.val, json->key.count) == 0);
    free(key_val);

    float val_val;
    RANDOM_ACCESS_FREAD_OR_FAIL(&val_val, sizeof(float), json_entity.val_ptr, file);
    assert(val_val == json->value.float_val);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return write_status;
}

static PerformStatus SectionExtents_WriteBoolJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    PerformStatus write_status = section_extents_write(extents, json, parent_json_addr, 0, 0, &save_json_addr);
    assert(save_json_addr == section_header_size());

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity_t) - sizeof(json->value) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header.first_record_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header.last_item_ptr == section_header_size() + sizeof(json_value_entity_t));

    json_value_entity_t json_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_value_entity_t), save_json_addr, file);
    assert(json_entity.key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity.key_size == string_get_size(json->key));
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity.val_size == sizeof(json->value));
    assert(json_entity.dad_ptr == 500);
    assert(json_entity.bro_ptr == 0);
    assert(json_entity.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity.key_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity.key_size, json_entity.key_ptr, file);
    assert(strncmp(key_val, json->key.val, json->key.count) == 0);
    free(key_val);

    bool val_val;
    RANDOM_ACCESS_FREAD_OR_FAIL(&val_val, sizeof(bool), json_entity.val_ptr, file);
    assert(val_val == json->value.bool_val);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return write_status;
}

static PerformStatus SectionExtents_WriteStringJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    string_t str;
    string_ctor(&str, "hello world!", 12);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    PerformStatus write_status = section_extents_write(extents, json, parent_json_addr, 0, 0, &save_json_addr);
    assert(save_json_addr == section_header_size());

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity_t) - string_get_size(json->value.string_val) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header.first_record_ptr == (SECTION_SIZE - string_get_size(json->value.string_val) - string_get_size(json->key)));
    assert(header.last_item_ptr == section_header_size() + sizeof(json_value_entity_t));

    json_value_entity_t json_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_entity), save_json_addr, file);
    assert(json_entity.key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity.key_size == string_get_size(json->key));
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->key) - string_get_size(json->value.string_val));
    assert(json_entity.val_size == string_get_size(json->value.string_val));
    assert(json_entity.dad_ptr == 500);
    assert(json_entity.bro_ptr == 0);
    assert(json_entity.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity.key_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity.key_size, json_entity.key_ptr, file);
    assert(strncmp(key_val, json->key.val, json->key.count) == 0);
    free(key_val);

    char *val_val = my_malloc_array(char, json_entity.val_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(val_val, json_entity.val_size, json_entity.val_ptr, file);
    assert(strncmp(val_val, json->value.string_val.val, json->value.string_val.count) == 0);
    free(val_val);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return write_status;
}

static PerformStatus SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    string_t str;
    string_ctor(&str, "hello world!", 12);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    extents->header.free_space = 5;
    PerformStatus write_status = section_extents_write(extents, json, parent_json_addr, 0, 0, &save_json_addr);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return write_status;
}

/*
"parent" : {
    "bool"  : false,
    "int32" : 5,
    "float" : 5.5,
    "string": "hello"
}
*/
static PerformStatus SectionExtents_WriteObjectJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    fileoff_t parent_json_addr = 1000;
    fileoff_t object_json_addr;

    JSON_VALUE_INIT(TYPE_OBJECT, object_json, "parent", NULL);

    section_extents_write(extents, object_json, parent_json_addr, 0, 0, &object_json_addr);

    json_value_entity_t object_json_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&object_json_entity, sizeof(json_value_entity_t), object_json_addr, file);
    assert(object_json_entity.key_ptr == SECTION_SIZE - string_get_size(object_json->key));
    assert(object_json_entity.key_size == string_get_size(object_json->key));
    assert(object_json_entity.val_ptr == 0);
    assert(object_json_entity.val_size == 0);
    assert(object_json_entity.dad_ptr == 1000);
    assert(object_json_entity.bro_ptr == 0);
    assert(object_json_entity.son_ptr == 0);
    assert(object_json_entity.type == TYPE_OBJECT);

    json_value_dtor(object_json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

void test_extents_write()
{
    assert(SectionExtents_DefaultCtor_InvokeHeaderCtor() == OK);

    assert(SectionExtents_InvokeSync_InvokeHeaderSync() == OK);

    assert(SectionExtents_WriteInt32JsonValue_Successful() == OK);
    assert(SectionExtents_WriteFloatJsonValue_Successful() == OK);
    assert(SectionExtents_WriteBoolJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed() == FAILED);

    assert(SectionExtents_WriteObjectJsonValue_Successful() == OK);
}
