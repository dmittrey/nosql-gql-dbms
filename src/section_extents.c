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
void section_extents_dtor(section_extents_t *section)
{
    static const char zeros[SECTION_SIZE];

    size_t prev = ftell(section->header.filp);

    fseek(section->header.filp, section->header.section_offset, SEEK_SET);
    fwrite(&zeros, sizeof(char), SECTION_SIZE, section->header.filp);
    section_header_dtor((section_header_t *)section);

    fseek(section->header.filp, prev, SEEK_SET);
}

PerformStatus section_extents_sync(section_extents_t *section)
{
    return section_header_sync((section_header_t *)section);
}

PerformStatus section_extents_write(section_extents_t *section, json_value_t *json, fileoff_t *parent_json_addr, fileoff_t *save_addr)
{
    if (section->header.free_space >= json_value_get_item_size(json) + json_value_get_record_size(json))
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

        sectoff_t parent_node_last_item_ptr = section->header.last_item_ptr;
        sectoff_t child_node_last_item_ptr = section->header.last_item_ptr + json->object.attributes_count * 3 * SECTION_EXTENTS_ITEM_SIZE;

        for (size_t i = 0; i < json->object.attributes_count; i++)
        {
            sectoff_t attr_key_ptr = 0;
            sectoff_t attr_val_ptr = 0;

            kv *cur_attribute = json->object.attributes[i];

            // Write attr key, save ptr and size into entity
            section_extents_write_in_record(section, string_get_size(cur_attribute->key), (void *)cur_attribute->key.val, &attr_key_ptr);

            // Write attr value, save ptr(We can fetch size via type)
            section->header.last_item_ptr = child_node_last_item_ptr;
            section->header.free_space = section->header.first_record_ptr - child_node_last_item_ptr;
            section_extents_write(section, json->object.attributes[i]->value, save_addr, &attr_val_ptr);
            child_node_last_item_ptr = section->header.last_item_ptr;
            section->header.free_space = section->header.first_record_ptr - child_node_last_item_ptr;

            // Save items of attribute(key_size, key_ptr, val_ptr)
            section->header.last_item_ptr = parent_node_last_item_ptr;
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &cur_attribute->key.count);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &attr_key_ptr);
            section_extents_write_in_item(section, SECTION_EXTENTS_ITEM_SIZE, &attr_val_ptr);
            parent_node_last_item_ptr = section->header.last_item_ptr;
        }

        return OK;
    }

    return FAILED;
}
PerformStatus section_extents_read(section_extents_t *section, sectoff_t offset, json_value_t *json)
{
    size_t prev = ftell(section->header.filp);

    // Read json entity
    json_value_entity *json_entity = my_malloc(json_value_entity);
    FSEEK_OR_FAIL(section->header.filp, offset);
    FREAD_OR_FAIL(&json_entity->attr_count, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);
    FREAD_OR_FAIL(&json_entity->type, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);
    FREAD_OR_FAIL(&json_entity->val_size, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);
    FREAD_OR_FAIL(&json_entity->val_ptr, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);
    FREAD_OR_FAIL(&json_entity->parent, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);
    FREAD_OR_FAIL(&json_entity->next, SECTION_EXTENTS_ITEM_SIZE, section->header.filp);

    // Read json attributes entities
    json->object.attributes = my_malloc_array(kv *, json_entity->attr_count);

    attr_entity *attribute_entity = my_malloc(attr_entity);
    json_value_t *attribute_value = my_malloc(json_value_t);
    kv *kv_ = my_malloc(kv);

    for (size_t i = 0; i < json_entity->attr_count; i++)
    {
        FREAD_OR_FAIL(attribute_entity, sizeof(attr_entity), section->header.filp);

        char attribute_key[attribute_entity->key_size];
        FSEEK_OR_FAIL(section->header.filp, attribute_entity->key_ptr);
        FREAD_OR_FAIL(&attribute_key, attribute_entity->key_size, section->header.filp);

        section_extents_read(section, attribute_entity->value_ptr, &attribute_value);

        kv_->key = string_ctor(attribute_key);
        kv_->value = attribute_value;
        json->object.attributes[i] = kv_;
    }

    // Read json value
    if (json_entity->type == TYPE_STRING)
    {
        json->value.string_val.val = my_malloc_array(char, json_entity->val_size);
        json->value.string_val.count = json_entity->val_size;
        FSEEK_OR_FAIL(section->header.filp, json_entity->val_ptr);
        FREAD_OR_FAIL(json->value.string_val.val, json->value.string_val.count, section->header.filp);
    }
    else
    {
        FREAD_OR_FAIL(&json->value, json_entity->val_size, section->header.filp);
    }

    json->object.attributes_count = json_entity->attr_count;
    json->type = json_entity->type;

    return OK;
}

PerformStatus section_documents_update(section_extents_t *, json_value_t *);
PerformStatus section_documents_delete(section_extents_t *, json_value_t *);

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