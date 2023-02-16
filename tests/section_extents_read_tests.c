#include <assert.h>
#include <string.h>

#include "section/section_extents.h"

#include "entity/json_value_entity.h"

static const char *test_file_name = "/Users/dmitry/Desktop/low-level-programming/test.bin";

static PerformStatus SectionExtents_ReadStringJsonValue_ReturnsValidJson()
{
    FILE *file = fopen(test_file_name, "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json_1 = json_value_new();
    json_value_ctor(json_1, TYPE_STRING, 0);
    json_1->value.string_val = string_ctor("Иван");

    json_value_t *json_2 = json_value_new();
    json_value_ctor(json_2, TYPE_STRING, 0);
    json_2->value.string_val = string_ctor("Иванов");

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

    json_value_t *readed_json_1 = my_malloc(json_value_t);
    section_extents_read(extents, save_json_1_addr, readed_json_1);
    free(readed_json_1);

    json_value_t *readed_json_2 = my_malloc(json_value_t);
    section_extents_read(extents, save_json_2_addr, readed_json_2);
    free(readed_json_2);

    json_value_dtor(json_1);
    json_value_dtor(json_2);
    section_extents_dtor(extents);
    fclose(file);

    return OK;
}

static PerformStatus SectionExtents_ReadObjectJsonValue_ReturnsValidJson()
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

    struct kv *kv_1 = my_malloc(struct kv);
    kv_1->key = string_ctor("firstName");
    kv_1->value = first_json;
    json_obj->object.attributes[0] = kv_1;

    struct kv *kv_2 = my_malloc(struct kv);
    kv_2->key = string_ctor("secondName");
    kv_2->value = second_json;
    json_obj->object.attributes[1] = kv_2;

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    section_extents_write(extents, json_obj, &parent_json_addr, &save_json_addr);

    json_value_t *readed_json_1 = my_malloc(json_value_t);
    section_extents_read(extents, save_json_addr, readed_json_1);

    assert(readed_json_1->type == TYPE_OBJECT);
    assert(readed_json_1->object.attributes_count == 2);

    assert(readed_json_1->object.attributes[0]->key.count == 9);
    assert(strcmp(readed_json_1->object.attributes[0]->key.val, "firstName") == 0);

    assert(readed_json_1->object.attributes[1]->key.count == 10);
    assert(strcmp(readed_json_1->object.attributes[1]->key.val, "secondName") == 0);

    assert(strcmp(readed_json_1->object.attributes[0]->value->value.string_val.val, "Иван") == 0);
    assert(strcmp(readed_json_1->object.attributes[1]->value->value.string_val.val, "Иванов") == 0);

    return OK;
}

void test_extents_read()
{
    assert(SectionExtents_ReadStringJsonValue_ReturnsValidJson() == OK);
    assert(SectionExtents_ReadObjectJsonValue_ReturnsValidJson() == OK);
}
