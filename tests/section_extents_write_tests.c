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

    assert(extents->header.free_space == (SECTION_SIZE - section_header_size(extents->header)));
    assert(extents->header.next == 0); // Next section is undefined
    assert(extents->header.last_item_ptr == section_header_size(extents->header));
    assert(extents->header.first_record_ptr == SECTION_SIZE);

    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_InvokeSync_InvokeHeaderSync()
{
    FILE *file = fopen(test_file_name, "r+");
    section_extents_t *extents = section_extents_new();

    extents->header.free_space = 5;
    extents->header.next = 6;
    extents->header.last_item_ptr = 7;
    extents->header.first_record_ptr = 8;
    extents->header.section_offset = 0;
    extents->header.filp = file;

    section_extents_sync(extents);

    section_header_entity header;
    FREAD_OR_FAIL(&header, sizeof(section_header_entity), file);

    assert(header.free_space == 5);
    assert(header.next_ptr == 6);
    assert(header.last_item_ptr == 7);
    assert(header.first_record_ptr == 8);

    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteInt32sonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_INT32, 0);
    json->value.int32_val = 5;

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity) - sizeof(json->value)));
    assert(header.first_record_ptr = (SECTION_SIZE - sizeof(json->value)));
    assert(header.last_item_ptr = section_header_size() + sizeof(json_value_entity));

    json_value_entity json_entity;
    int32_t json_value;

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_entity), save_json_addr, file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == TYPE_INT32);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 500);
    assert(json_entity.next == 0);

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_value, sizeof(json->value), json_entity.val_ptr, file);
    assert(json_value == 5);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteFloatsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_FLOAT, 0);
    json->value.float_val = 5.5;

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity) - sizeof(json->value)));
    assert(header.first_record_ptr = (SECTION_SIZE - sizeof(json->value)));
    assert(header.last_item_ptr = section_header_size() + sizeof(json_value_entity));

    json_value_entity json_entity;
    float json_value;

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_entity), save_json_addr, file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == TYPE_FLOAT);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 500);
    assert(json_entity.next == 0);

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_value, sizeof(json->value), json_entity.val_ptr, file);
    assert(json_value == 5.5);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteBoolsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_BOOL, 0);
    json->value.bool_val = true;

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity) - sizeof(json->value)));
    assert(header.first_record_ptr = (SECTION_SIZE - sizeof(json->value)));
    assert(header.last_item_ptr = section_header_size() + sizeof(json_value_entity));

    json_value_entity json_entity;
    bool json_value;

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_entity, sizeof(json_entity), save_json_addr, file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == TYPE_BOOL);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 500);
    assert(json_entity.next == 0);

    RANDOM_ACCESS_FREAD_OR_FAIL(&json_value, sizeof(json->value), json_entity.val_ptr, file);
    assert(json_value == true);

    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteStringJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_STRING, 0);
    json->value.string_val = string_ctor("Hello world!");

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    assert(header.free_space == (SECTION_SIZE - section_header_size() - sizeof(json_value_entity) - string_get_size(json->value.string_val)));
    assert(header.first_record_ptr = (SECTION_SIZE - string_get_size(json->value.string_val)));
    assert(header.last_item_ptr = section_header_size() + sizeof(json_value_entity));

    json_value_entity json_entity;
    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);

    char *json_value = my_malloc_array(char, json_entity.val_size);
    FSEEK_OR_FAIL(file, json_entity.val_ptr);
    FREAD_OR_FAIL(json_value, json_entity.val_size, file);

    assert(json_entity.attr_count == 0);
    assert(json_entity.type == 2);
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->value.string_val));
    assert(json_entity.val_size == string_get_size(json->value.string_val));
    assert(json_entity.parent == 0);
    assert(json_entity.next == 0);

    assert(strcmp(json_value, json->value.string_val.val) == 0);

    json_value_dtor(json);
    free(json_value);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteObjectJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json_obj = json_value_new();
    json_value_ctor(json_obj, TYPE_OBJECT, 2);

    json_value_t *first_json = json_value_new();
    json_value_ctor(first_json, TYPE_STRING, 0);
    first_json->value.string_val = string_ctor("Иван");

    json_value_t *second_json = json_value_new();
    json_value_ctor(second_json, TYPE_STRING, 0);
    second_json->value.string_val = string_ctor("Иванов");

    struct json_kv_t *kv_1 = my_malloc(struct json_kv_t);
    kv_1->key = string_ctor("firstName");
    kv_1->value = first_json;
    json_obj->object.attributes[0] = kv_1;

    struct json_kv_t *kv_2 = my_malloc(struct json_kv_t);
    kv_2->key = string_ctor("secondName");
    kv_2->value = second_json;
    json_obj->object.attributes[1] = kv_2;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json_obj, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    section_header_entity header;
    RANDOM_ACCESS_FREAD_OR_FAIL(&header, sizeof(section_header_entity), 0, file);
    /*
    size_t extimate =
    SECTION_SIZE - 8192 - 32 - 48 - 9 - 48 - 8 - 24 - 10 - 48 - 12 - 24
    section_header_size() - 32
    3 * sizeof(json_value_entity) - 3 * 48
    2 * sizeof(attr_entity) - 2 * 24
    string_get_size(json_obj->object.attributes[0]->key) - 9
    string_get_size(json_obj->object.attributes[1]->key) - 10
    string_get_size(json_obj->object.attributes[0]->value->value.string_val) - 8
    string_get_size(json_obj->object.attributes[1]->value->value.string_val) - 12
    */
    assert(header.free_space == (SECTION_SIZE - section_header_size() - 3 * sizeof(json_value_entity) - 2 * sizeof(attr_entity) -
                                 string_get_size(json_obj->object.attributes[0]->key) - string_get_size(json_obj->object.attributes[1]->key) -
                                 string_get_size(json_obj->object.attributes[0]->value->value.string_val) - string_get_size(json_obj->object.attributes[1]->value->value.string_val)));
    assert(header.first_record_ptr == (SECTION_SIZE - string_get_size(json_obj->object.attributes[0]->key) - string_get_size(json_obj->object.attributes[1]->key) -
                                       string_get_size(json_obj->object.attributes[0]->value->value.string_val) - string_get_size(json_obj->object.attributes[1]->value->value.string_val)));
    assert(header.last_item_ptr == section_header_size() + 3 * sizeof(json_value_entity) + 2 * sizeof(attr_entity));

    json_value_entity json_entity;
    attr_entity json_attr_1_entity;
    attr_entity json_attr_2_entity;

    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_value_entity), file);
    FREAD_OR_FAIL(&json_attr_1_entity, sizeof(attr_entity), file);
    FREAD_OR_FAIL(&json_attr_2_entity, sizeof(attr_entity), file);

    char *json_attr_1_key = my_malloc_array(char, json_attr_1_entity.key_size);
    FSEEK_OR_FAIL(file, json_attr_1_entity.key_ptr);
    FREAD_OR_FAIL(json_attr_1_key, json_attr_1_entity.key_size, file);

    json_value_entity json_attr_1_value;
    FSEEK_OR_FAIL(file, json_attr_1_entity.value_ptr);
    FREAD_OR_FAIL(&json_attr_1_value, sizeof(json_value_entity), file);

    char *json_attr_1_value_string = my_malloc_array(char, json_attr_1_value.val_size);
    FSEEK_OR_FAIL(file, json_attr_1_value.val_ptr);
    FREAD_OR_FAIL(json_attr_1_value_string, json_attr_1_value.val_size, file);

    char *json_attr_2_key = my_malloc_array(char, json_attr_2_entity.key_size);
    FSEEK_OR_FAIL(file, json_attr_2_entity.key_ptr);
    FREAD_OR_FAIL(json_attr_2_key, json_attr_2_entity.key_size, file);

    json_value_entity json_attr_2_value;
    FSEEK_OR_FAIL(file, json_attr_2_entity.value_ptr);
    FREAD_OR_FAIL(&json_attr_2_value, sizeof(json_value_entity), file);

    char *json_attr_2_value_string = my_malloc_array(char, json_attr_2_value.val_size);
    FSEEK_OR_FAIL(file, json_attr_2_value.val_ptr);
    FREAD_OR_FAIL(json_attr_2_value_string, json_attr_2_value.val_size, file);



    // Test json obj
    assert(json_entity.attr_count == json_obj->object.attributes_count);
    assert(json_entity.type == json_obj->type);
    assert(json_entity.val_ptr == 0);
    assert(json_entity.val_size == sizeof(json_obj->value));
    assert(json_entity.parent == 0);
    assert(json_entity.next == 0);

    // Test json obj first attribute internals
    assert(json_attr_1_entity.key_size == string_get_size(kv_1->key));
    assert(json_attr_1_entity.key_ptr == SECTION_SIZE - string_get_size(kv_1->key));
    assert(json_attr_1_entity.value_ptr == section_header_size() + sizeof(json_value_entity) + json_entity.attr_count * sizeof(attr_entity));
    // Test json obj second attribute internals
    assert(json_attr_2_entity.key_size == string_get_size(kv_2->key));
    assert(json_attr_2_entity.key_ptr == SECTION_SIZE - string_get_size(kv_1->key) - string_get_size(first_json->value.string_val) - string_get_size(kv_2->key));
    assert(json_attr_2_entity.value_ptr == section_header_size() + 2 * sizeof(json_value_entity) + json_entity.attr_count * sizeof(attr_entity));

    // Compare first attribute key
    assert(strcmp(json_attr_1_key, kv_1->key.val) == 0);
    // Compare first attribute value
    assert(strcmp(json_attr_1_value_string, first_json->value.string_val.val) == 0);

    // Compare second attribute key
    assert(strcmp(json_attr_2_key, kv_2->key.val) == 0);
    // Compare second attribute value
    assert(strcmp(json_attr_2_value_string, second_json->value.string_val.val) == 0);

    json_value_dtor(json_obj);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    extents->header.free_space = 1;

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_STRING, 0);
    json->value.string_val = string_ctor("Иван");

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    if (section_extents_write(extents, json, &parent_json_addr, &save_json_addr))
    {
        return FAILED;
    }

    json_value_dtor(json);
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
    assert(SectionExtents_WriteInt32sonValue_Successful() == OK);
    assert(SectionExtents_WriteFloatsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValue_Successful() == OK);
    assert(SectionExtents_WriteBoolsonValue_Successful() == OK);
    assert(SectionExtents_WriteObjectJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed() == FAILED);
}
