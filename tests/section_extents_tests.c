#include <assert.h>
#include <string.h>

#include "section/section_extents.h"

#include "entity/json_value_entity.h"

void SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    assert(extents->header.free_space == (SECTION_SIZE - section_header_size(extents->header)));
    assert(extents->header.next == 0); // Next section is undefined
    assert(extents->header.last_item_ptr == section_header_size(extents->header));
    assert(extents->header.first_record_ptr == SECTION_SIZE);

    section_extents_dtor(extents);
    fclose(file);
}

bool SectionExtents_InvokeSync_InvokeHeaderSync()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    section_extents_t *extents = section_extents_new();

    extents->header.free_space = 5;
    extents->header.next = 6;
    extents->header.last_item_ptr = 7;
    extents->header.first_record_ptr = 8;
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

    return true;
}

bool SectionExtents_WriteInt32JsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

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

    return true;
}

bool SectionExtents_WriteFloatJsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

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

    return true;
}

bool SectionExtents_WriteStringJsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

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

    return true;
}

bool SectionExtents_WriteBoolJsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

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

    return true;
}

bool SectionExtents_WriteObjectJsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.bin", "r+");

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
    fileoff_t parent_json_addr = 10000;

    section_extents_write(extents, json_obj, &parent_json_addr, &save_json_addr);
    // assert(save_json_addr == section_header_size(extents->header));

    // json_value_entity json_entity;
    // uint64_t kv_1_entity;
    // kv_entity kv_2_entity;

    // FSEEK_OR_FAIL(file, save_json_addr);
    // FREAD_OR_FAIL(&json_entity, sizeof(json_entity), file);
    // int a = ftell(file);
    // FREAD_OR_FAIL(&kv_1_entity, 8, file);
    // FREAD_OR_FAIL(&kv_1_entity, sizeof(kv_entity), file);
    // FREAD_OR_FAIL(&kv_2_entity, sizeof(kv_entity), file);

    // assert(json_entity.attr_count == 2);
    // assert(json_entity.type == 4);
    // assert(json_entity.val_ptr == 0);
    // assert(json_entity.parent == 10000);
    // assert(json_entity.next == 0);

    // assert(kv_1_entity.key_ptr == SECTION_SIZE - string_get_size(kv_1->key));
    // assert(kv_1_entity.key_size == string_get_size(kv_1->key));
    // assert(kv_1_entity.value_ptr == SECTION_SIZE - string_get_size(kv_1->key) - string_get_size(first_json->value.string_val));

    // float json_value;
    // FSEEK_OR_FAIL(file, json_entity.val_ptr);
    // FREAD_OR_FAIL(&json_value, sizeof(json->value), file);
    // assert(json_value == false);

    section_extents_dtor(extents);
    fclose(file);

    return true;
}

void SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
}

int main()
{
    // SectionExtents_DefaultCtor_InvokeHeaderCtor();
    // assert(SectionExtents_InvokeSync_InvokeHeaderSync() == true);
    // assert(SectionExtents_WriteInt32JsonValue_Successful() == true);
    // assert(SectionExtents_WriteFloatJsonValue_Successful() == true);
    // assert(SectionExtents_WriteStringJsonValue_Successful() == true);
    // assert(SectionExtents_WriteBoolJsonValue_Successful() == true);
    SectionExtents_WriteObjectJsonValue_Successful();
    // SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed();
}
