#include <assert.h>
#include <string.h>

#include "section/section_extents.h"

#include "entity/json_value_entity.h"

static const char *test_file_name = "/Users/dmitry/Desktop/low-level-programming/test.bin";

void SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    assert(extents->header.free_space == (SECTION_SIZE - section_header_size(extents->header)));
    assert(extents->header.next == 0); // Next section is undefined
    assert(extents->header.last_item_ptr == section_header_size(extents->header));
    assert(extents->header.first_record_ptr == SECTION_SIZE);

    section_extents_dtor(extents);
    fclose(file);
}

PerformStatus SectionExtents_InvokeSync_InvokeHeaderSync()
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
    // TODO Refactor to macro
    sectoff_t file_free_space;
    FREAD_OR_FAIL(&file_free_space, sizeof(sectoff_t), file);
    assert(file_free_space == 5);

    sectoff_t file_next;
    FREAD_OR_FAIL(&file_next, sizeof(sectoff_t), file);
    assert(file_next == 6);

    sectoff_t file_last_item_ptr;
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(sectoff_t), file);
    assert(file_last_item_ptr == 7);

    sectoff_t file_first_record_ptr;
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(sectoff_t), file);
    assert(file_first_record_ptr == 8);

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

PerformStatus SectionExtents_WriteInt32JsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json->type = TYPE_INT32;
    json->value.int32_val = 5;
    json->object.attributes_count = 0;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    json_value_entity json_entity;
    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == 0);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 0);
    assert(json_entity.next == 0);

    int32_t json_value;
    FSEEK_OR_FAIL(file, json_entity.val_ptr);
    FREAD_OR_FAIL(&json_value, sizeof(json->value), file);
    assert(json_value == 5);

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

PerformStatus SectionExtents_WriteFloatJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json->type = TYPE_FLOAT;
    json->value.float_val = 5.5;
    json->object.attributes_count = 0;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 500;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    json_value_entity json_entity;
    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == 1);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 500);
    assert(json_entity.next == 0);

    float json_value;
    FSEEK_OR_FAIL(file, json_entity.val_ptr);
    FREAD_OR_FAIL(&json_value, sizeof(json->value), file);
    assert(json_value == 5.5);

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

PerformStatus SectionExtents_WriteStringJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json->type = TYPE_STRING;
    json->value.string_val = (string_t){.count = 12, .val = "Hello world!"};
    json->object.attributes_count = 0;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 10000;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    json_value_entity json_entity;
    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == 2);
    assert(json_entity.val_ptr == SECTION_SIZE - string_get_size(json->value.string_val));
    assert(json_entity.parent == 10000);
    assert(json_entity.next == 0);

    char *json_value = my_malloc_array(char, 12);
    FSEEK_OR_FAIL(file, 8180);
    FREAD_OR_FAIL(json_value, string_get_size(json->value.string_val), file);
    assert(strcmp(json_value, "Hello world!") == 0);

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

PerformStatus SectionExtents_WriteBoolJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json->type = TYPE_BOOL;
    json->value.float_val = false;
    json->object.attributes_count = 0;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 10000;

    section_extents_write(extents, json, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    json_value_entity json_entity;
    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    assert(json_entity.attr_count == 0);
    assert(json_entity.type == 3);
    assert(json_entity.val_ptr == SECTION_SIZE - sizeof(json->value));
    assert(json_entity.parent == 10000);
    assert(json_entity.next == 0);

    float json_value;
    FSEEK_OR_FAIL(file, json_entity.val_ptr);
    FREAD_OR_FAIL(&json_value, sizeof(json->value), file);
    assert(json_value == false);

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

PerformStatus SectionExtents_WriteObjectJsonValue_Successful()
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

    kv *kv_1 = my_malloc(kv);
    kv_1->key = string_ctor("firstName");
    kv_1->value = first_json;
    json_obj->object.attributes[0] = kv_1;

    kv *kv_2 = my_malloc(kv);
    kv_2->key = string_ctor("secondName");
    kv_2->value = second_json;
    json_obj->object.attributes[1] = kv_2;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json_obj, &parent_json_addr, &save_json_addr);
    assert(save_json_addr == section_header_size(extents->header));

    json_value_entity json_entity;
    attr_entity json_attr_1_entity;
    attr_entity json_attr_2_entity;

    FSEEK_OR_FAIL(file, save_json_addr);
    FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    FREAD_OR_FAIL(&json_attr_1_entity, sizeof(attr_entity), file);
    FREAD_OR_FAIL(&json_attr_2_entity, sizeof(attr_entity), file);

    char *json_attr_1_key = my_malloc_array(char, json_attr_1_entity.key_size);
    FSEEK_OR_FAIL(file, json_attr_1_entity.key_ptr);
    FREAD_OR_FAIL(json_attr_1_key, json_attr_1_entity.key_size, file);

    json_value_entity json_attr_1_value;
    FSEEK_OR_FAIL(file, json_attr_1_entity.value_ptr);
    FREAD_OR_FAIL(&json_attr_1_value, sizeof(json_value_entity), file);

    char *json_attr_1_value_string = my_malloc_array(char, string_get_size(first_json->value.string_val));
    FSEEK_OR_FAIL(file, json_attr_1_value.val_ptr);
    FREAD_OR_FAIL(json_attr_1_value_string, string_get_size(first_json->value.string_val), file);

    char *json_attr_2_key = my_malloc_array(char, json_attr_2_entity.key_size);
    FSEEK_OR_FAIL(file, json_attr_2_entity.key_ptr);
    FREAD_OR_FAIL(json_attr_2_key, json_attr_2_entity.key_size, file);

    json_value_entity json_attr_2_value;
    FSEEK_OR_FAIL(file, json_attr_2_entity.value_ptr);
    FREAD_OR_FAIL(&json_attr_2_value, sizeof(json_value_entity), file);

    char *json_attr_2_value_string = my_malloc_array(char, string_get_size(second_json->value.string_val));
    FSEEK_OR_FAIL(file, json_attr_2_value.val_ptr);
    FREAD_OR_FAIL(json_attr_2_value_string, string_get_size(second_json->value.string_val), file);

    // Test json obj
    assert(json_entity.attr_count == json_obj->object.attributes_count);
    assert(json_entity.type == json_obj->type);
    assert(json_entity.val_ptr == 0);
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

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

bool SectionExtents_WriteArrayJsonValue_Successful()
{
}

PerformStatus SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
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

    if (section_extents_write(extents, json, &parent_json_addr, &save_json_addr)) {
        return FAILED;
    }

    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

int main()
{
    SectionExtents_DefaultCtor_InvokeHeaderCtor();
    assert(SectionExtents_InvokeSync_InvokeHeaderSync() == OK);
    assert(SectionExtents_WriteInt32JsonValue_Successful() == OK);
    assert(SectionExtents_WriteFloatJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValue_Successful() == OK);
    assert(SectionExtents_WriteBoolJsonValue_Successful() == OK);
    assert(SectionExtents_WriteObjectJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed() == FAILED);
}
