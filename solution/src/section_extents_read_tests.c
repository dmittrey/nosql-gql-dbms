#include <assert.h>
#include <string.h>

#include "section/section_extents.h"

#include "entity/json_value_entity.h"

static const char *test_file_name = "test.bin";

static PerformStatus SectionExtents_ReadInt32JsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_INT32, 0);
    json->value.int32_val = 5;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    if (section_extents_write(extents, json, &parent_json_addr, &save_json_addr))
    {
        return FAILED;
    }

    json_value_t *readed_json = json_value_new();
    json_value_ctor(readed_json, TYPE_STRING, 0);
    section_extents_read(extents, save_json_addr, readed_json);
    assert(readed_json->type == TYPE_INT32);
    assert(readed_json->value.int32_val == 5);

    json_value_dtor(readed_json);
    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_ReadFloatJsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_FLOAT, 0);
    json->value.float_val = 5.5;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    if (section_extents_write(extents, json, &parent_json_addr, &save_json_addr))
    {
        return FAILED;
    }

    json_value_t *readed_json = json_value_new();
    json_value_ctor(readed_json, TYPE_STRING, 0);
    section_extents_read(extents, save_json_addr, readed_json);
    assert(readed_json->type == TYPE_FLOAT);
    assert(readed_json->value.float_val == 5.5);

    json_value_dtor(readed_json);
    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_ReadBoolJsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    json_value_ctor(json, TYPE_BOOL, 0);
    json->value.bool_val = true;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    if (section_extents_write(extents, json, &parent_json_addr, &save_json_addr))
    {
        return FAILED;
    }

    json_value_t *readed_json = json_value_new();
    json_value_ctor(readed_json, TYPE_STRING, 0);
    section_extents_read(extents, save_json_addr, readed_json);
    assert(readed_json->type == TYPE_BOOL);
    assert(readed_json->value.bool_val == true);

    json_value_dtor(readed_json);
    json_value_dtor(json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_ReadStringJsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json_1 = json_value_new();
    json_value_ctor(json_1, TYPE_STRING, 0);
    json_1->value.string_val = string_ctor("Иван", 4);

    json_value_t *json_2 = json_value_new();
    json_value_ctor(json_2, TYPE_STRING, 0);
    json_2->value.string_val = string_ctor("Иванов", 12);

    fileoff_t save_json_1_addr = 0;
    fileoff_t save_json_2_addr = 0;
    fileoff_t parent_json_addr = 0;

    if (section_extents_write(extents, json_1, &parent_json_addr, &save_json_1_addr))
    {
        return FAILED;
    }

    if (section_extents_write(extents, json_2, &parent_json_addr, &save_json_2_addr))
    {
        return FAILED;
    }

    json_value_t *readed_json_1 = json_value_new();
    json_value_ctor(readed_json_1, TYPE_STRING, 0);
    section_extents_read(extents, save_json_1_addr, readed_json_1);
    assert(readed_json_1->type == TYPE_STRING);
    assert(readed_json_1->value.string_val.count == 8);
    assert(strncmp(readed_json_1->value.string_val.val, json_1->value.string_val.val, json_1->value.string_val.count) == 0);
    json_value_dtor(readed_json_1);

    json_value_t *readed_json_2 = json_value_new();
    json_value_ctor(readed_json_2, TYPE_STRING, 0);
    section_extents_read(extents, save_json_2_addr, readed_json_2);
    assert(readed_json_2->type == TYPE_STRING);
    assert(readed_json_2->value.string_val.count == 12);
    assert(strncmp(readed_json_2->value.string_val.val, json_2->value.string_val.val, json_2->value.string_val.count) == 0);
    json_value_dtor(readed_json_2);

    json_value_dtor(json_1);
    json_value_dtor(json_2);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static PerformStatus SectionExtents_ReadObjectJsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "w+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json_obj = json_value_new();
    json_value_ctor(json_obj, TYPE_OBJECT, 2);

    json_value_t *first_json = json_value_new();
    json_value_ctor(first_json, TYPE_STRING, 0);
    first_json->value.string_val = string_ctor("Иван", 4);

    json_value_t *second_json = json_value_new();
    json_value_ctor(second_json, TYPE_STRING, 0);
    second_json->value.string_val = string_ctor("Иванов", 12);

    struct json_kv_t *kv_1 = my_malloc(struct json_kv_t);
    kv_1->key = string_ctor("firstName", 9);
    kv_1->value = first_json;
    json_obj->object.attributes[0] = kv_1;

    struct json_kv_t *kv_2 = my_malloc(struct json_kv_t);
    kv_2->key = string_ctor("secondName", 10);
    kv_2->value = second_json;
    json_obj->object.attributes[1] = kv_2;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json_obj, &parent_json_addr, &save_json_addr);

    json_value_t *readed_json = json_value_new();
    json_value_ctor(readed_json, TYPE_STRING, 0);
    section_extents_read(extents, save_json_addr, readed_json);

    assert(readed_json->type == TYPE_OBJECT);
    assert(readed_json->object.attributes_count == 2);

    assert(readed_json->object.attributes[0]->key.count == 9);
    assert(strcmp(readed_json->object.attributes[0]->key.val, "firstName") == 0);

    struct json_kv_t *kv = readed_json->object.attributes[1];

    assert(readed_json->object.attributes[1]->key.count == 10);
    assert(strcmp(readed_json->object.attributes[1]->key.val, "secondName") == 0);

    assert(strcmp(readed_json->object.attributes[0]->value->value.string_val.val, "Иван") == 0);
    assert(strcmp(readed_json->object.attributes[1]->value->value.string_val.val, "Иванов") == 0);

    json_value_dtor(json_obj);
    json_value_dtor(readed_json);
    section_extents_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

void test_extents_read()
{
    assert(SectionExtents_ReadStringJsonValue_ReturnsValidJson() == OK);
    assert(SectionExtents_ReadInt32JsonValue_ReturnsValidJson() == OK);
    assert(SectionExtents_ReadFloatJsonValue_ReturnsValidJson() == OK);
    assert(SectionExtents_ReadBoolJsonValue_ReturnsValidJson() == OK);
    assert(SectionExtents_ReadObjectJsonValue_ReturnsValidJson() == OK);
}
