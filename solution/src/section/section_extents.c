#include "utils.h"
#include "table.h"

#include "section/section_header.h"
#include "section/section_extents.h"

#include "entity/json_value_entity.h"

static PerformStatus section_extents_write_in_item(section_extents_t *, size_t, void *);
static PerformStatus section_extents_write_in_record(section_extents_t *, size_t, void *, fileoff_t *);

section_extents_t *section_extents_new()
{
    return my_malloc(section_extents_t);
}

void section_extents_ctor(section_extents_t *section, fileoff_t offset, FILE *filp)
{
    section_header_ctor((section_header_t *)section, offset, filp);
}
PerformStatus section_extents_dtor(section_extents_t *section)
{
    static const char zeros[SECTION_SIZE];

    size_t prev = ftell(section->header.filp);

    if (fseek(section->header.filp, section->header.section_offset, SEEK_SET))
    {
        return FAILED;
    }
    if (fwrite(&zeros, sizeof(char), SECTION_SIZE, section->header.filp))
    {
        return FAILED;
    }

    if (fseek(section->header.filp, prev, SEEK_SET))
    {
        return FAILED;
    }

    section_header_dtor((section_header_t *)section);

    return OK;
}

PerformStatus section_extents_sync(section_extents_t *section)
{
    return section_header_sync((section_header_t *)section);
}

PerformStatus section_extents_write(section_extents_t *section, json_value_t *json, fileoff_t *parent_json_addr, fileoff_t *save_addr)
{
    size_t json_size = json_value_get_serialization_size(json);

    if (section->header.free_space >= json_size)
    {
        fileoff_t json_val_ptr = 0;

        *save_addr = section->header.last_item_ptr;

        if (json->type == TYPE_STRING)
        {
            section_extents_write_in_record(section, string_get_size(json->value.string_val), (void *)json->value.string_val.val, &json_val_ptr);
        }
        else if (json->type != TYPE_OBJECT)
        {
            section_extents_write_in_record(section, sizeof(json->value), &json->value, &json_val_ptr);
        }

        section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &json->object.attributes_count);
        section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &json->type);
        if (json->type == TYPE_STRING)
        {
            size_t size = string_get_size(json->value.string_val);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &size);
        }
        else
        {
            size_t size = sizeof(json->value);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &size);
        }
        section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &json_val_ptr);
        section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, parent_json_addr);
        section_header_shift_last_item_ptr((section_header_t *)section, SECTION_EXTENTS_ITEM_SIZE); // Hold gap for next json addr
        section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &json_size);

        sectoff_t parent_node_last_item_ptr = section->header.last_item_ptr;
        sectoff_t child_node_last_item_ptr = section->header.last_item_ptr + json->object.attributes_count * sizeof(attr_entity);

        for (size_t i = 0; i < json->object.attributes_count; i++)
        {
            sectoff_t attr_key_ptr = 0;
            sectoff_t attr_val_ptr = 0;

            struct json_kv_t *cur_attribute = json->object.attributes[i];

            // Write attr key, save ptr and size into entity
            section_extents_write_in_record(section, string_get_size(cur_attribute->key), (void *)cur_attribute->key.val, &attr_key_ptr);

            // Write attr value, save ptr(We can fetch size via type)
            section->header.last_item_ptr = child_node_last_item_ptr;
            section_extents_write(section, json->object.attributes[i]->value, save_addr, &attr_val_ptr);
            child_node_last_item_ptr = section->header.last_item_ptr;

            // Save items of attribute(key_size, key_ptr, val_ptr)
            section->header.last_item_ptr = parent_node_last_item_ptr;
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &cur_attribute->key.count);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &attr_key_ptr);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &attr_val_ptr);
            parent_node_last_item_ptr = section->header.last_item_ptr;
        }

        section->header.last_item_ptr = child_node_last_item_ptr;
        section_extents_sync(section);
        return OK;
    }

    return FAILED;
}
PerformStatus section_extents_read(section_extents_t *section, sectoff_t offset, json_value_t *json)
{
    // Allocate buffers
    json_value_entity *json_entity = my_malloc(json_value_entity);

    attr_entity *attribute_entity = my_malloc(attr_entity);

    // Save prev filp
    size_t prev = ftell(section->header.filp);

    // Read json entity
    RANDOM_ACCESS_FREAD_OR_FAIL(json_entity, sizeof(json_value_entity), offset, section->header.filp);

    json->object.attributes = ((struct json_kv_t **)malloc(sizeof(struct json_kv_t *) * json_entity->attr_count));

    for (size_t i = 0; i < json_entity->attr_count; i++)
    {
        RANDOM_ACCESS_FREAD_OR_FAIL(attribute_entity, sizeof(attr_entity), offset + sizeof(json_value_entity) + i * sizeof(attr_entity), section->header.filp);

        char *attribute_key = my_malloc_array(char, attribute_entity->key_size);
        RANDOM_ACCESS_FREAD_OR_FAIL(attribute_key, attribute_entity->key_size, attribute_entity->key_ptr, section->header.filp);

        json_value_t *attribute_value = json_value_new();
        json_value_ctor(attribute_value, TYPE_OBJECT, 0);
        section_extents_read(section, attribute_entity->value_ptr, attribute_value);

        struct json_kv_t *kv = my_malloc(struct json_kv_t);
        string_new(&kv->key);
        string_ctor(&kv->key , attribute_key, attribute_entity->key_size);
        kv->value = attribute_value;
        json->object.attributes[i] = kv;

        free(attribute_key);
    }

    // Read json value
    if (json_entity->type == TYPE_STRING)
    {
        json->value.string_val.val = my_malloc_array(char, json_entity->val_size);
        json->value.string_val.count = json_entity->val_size;
        RANDOM_ACCESS_FREAD_OR_FAIL(json->value.string_val.val, json->value.string_val.count, json_entity->val_ptr, section->header.filp);
    }
    else
    {
        RANDOM_ACCESS_FREAD_OR_FAIL(&json->value, json_entity->val_size, json_entity->val_ptr, section->header.filp);
    }

    json->object.attributes_count = json_entity->attr_count;
    json->type = json_entity->type;

    FSEEK_OR_FAIL(section->header.filp, prev);

    free(attribute_entity);
    free(json_entity);

    return OK;
}

/*
    Нужно проверить, хватит ли места поменять запись
        - Прочитать старую
        - Оценить длину сериализации
        - Оценить длину сериализации новой
        - Если получится то перезаписать запись
*/
PerformStatus section_documents_update(section_extents_t *section, sectoff_t offset, json_value_t *new_json)
{
    size_t new_json_size = json_value_get_serialization_size(new_json);

    fileoff_t save_json_addr = 0;
    fileoff_t parent_json_addr = 0;

    json_value_t *readed_json = my_malloc(json_value_t);
    section_extents_read(section, save_json_addr, readed_json);

    size_t old_json_size = json_value_get_serialization_size(readed_json);

    if (section->header.free_space + old_json_size - new_json_size >= 0)
    {
        fileoff_t parent_json_addr = 0;
        fileoff_t save_addr = 0;

        if (section_extents_write(section, new_json, &parent_json_addr, &save_addr))
        {
            return FAILED;
        }
    }

    free(readed_json);
    return OK;
}

PerformStatus section_documents_delete(section_extents_t *section, sectoff_t offset)
{
    json_value_entity entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&entity, SECTION_EXTENTS_ITEM_SIZE, offset, section->header.filp);

    section_header_shift_last_item_ptr((section_header_t *)section, offset);
    section_header_shift_first_record_ptr((section_header_t *)section, entity.val_ptr + entity.val_size);

    return OK;
}

static PerformStatus section_extents_write_in_item(section_extents_t *section, size_t data_size, void *data_ptr)
{
    long prev_ptr = ftell(section->header.filp);

    FSEEK_OR_FAIL(section->header.filp, section->header.last_item_ptr);
    FWRITE_OR_FAIL(data_ptr, data_size, section->header.filp);
    section_header_shift_last_item_ptr((section_header_t *)section, data_size);

    FSEEK_OR_FAIL(section->header.filp, prev_ptr);

    return OK;
}

static PerformStatus section_extents_write_in_record(section_extents_t *section, size_t data_size, void *data_ptr, fileoff_t *save_addr)
{
    long prev_ptr = ftell(section->header.filp);

    section_header_shift_first_record_ptr((section_header_t *)section, -1 * data_size);
    FSEEK_OR_FAIL(section->header.filp, section->header.first_record_ptr);
    FWRITE_OR_FAIL(data_ptr, data_size, section->header.filp); // Bad memory access TODO see memory alloc

    *save_addr = section->header.first_record_ptr;

    FSEEK_OR_FAIL(section->header.filp, prev_ptr);

    return OK;
}