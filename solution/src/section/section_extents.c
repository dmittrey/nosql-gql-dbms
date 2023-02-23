#include <string.h>

#include "utils.h"
#include "table.h"

#include "section/section_header.h"
#include "section/section_extents.h"

#include "entity/json_value_entity.h"

static PerformStatus section_extents_write_in_item(section_extents_t *const, const size_t, const void *const);
static PerformStatus section_extents_write_in_record(section_extents_t *const, const size_t, const void *const, fileoff_t *const);

section_extents_t *section_extents_new()
{
    return memset(my_malloc(section_extents_t), 0, sizeof(section_extents_t));
}

void section_extents_ctor(section_extents_t *const section, const fileoff_t offset, FILE *const filp)
{
    section_header_ctor((section_header_t *)section, offset, filp);
}
PerformStatus section_extents_dtor(section_extents_t *section)
{
    // size_t prev = ftell(section->header.filp);

    // void *zeros = my_malloc_array(char, SECTION_SIZE);
    // memset(zeros, '0', SECTION_SIZE);
    // RANDOM_ACCESS_FWRITE_OR_FAIL(zeros, SECTION_SIZE, section->header.section_offset, section->header.filp);
    // free(zeros);

    // FSEEK_OR_FAIL(section->header.filp, prev);

    section_header_dtor((section_header_t *)section);

    return OK;
}

PerformStatus section_extents_sync(section_extents_t *section)
{
    return section_header_sync((section_header_t *)section);
}

PerformStatus section_extents_write(section_extents_t *const section, const json_value_t *const json, const fileoff_t parent_addr, const fileoff_t bro_addr, const fileoff_t son_addr, fileoff_t *const save_json_addr)
{
    json_value_entity_t entity;
    json_value_entity_ctor(&entity, json, parent_addr, bro_addr, son_addr);

    if (section->header.free_space >= json_value_entity_get_physical_size(&entity))
    {
        *save_json_addr = section->header.last_item_ptr;

        section_extents_write_in_record(section, sizeof(char) * json->key.count, json->key.val, &entity.key_ptr);
        section_extents_write_in_record(section, json_value_get_val_size(json), json_value_get_val_ptr(json), &entity.val_ptr);

        section_extents_write_in_item(section, sizeof(json_value_entity_t), &entity);

        json_value_entity_dtor(&entity);

        return OK;
    }
    else
    {
        json_value_entity_dtor(&entity);

        return FAILED;
    }
}

PerformStatus section_extents_read(const section_extents_t *const section, const sectoff_t offset, json_value_t *const json)
{
    json_value_entity_t entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&entity, sizeof(json_value_entity_t), offset, section->header.filp);

    // Key parsing
    char *key = my_malloc_array(char, entity.key_size);
    RANDOM_ACCESS_FREAD_OR_FAIL(key, entity.key_size, entity.key_ptr, section->header.filp);

    json_value_ctor(json, TYPE_STRING, key, entity.key_size);

    free(key);

    if (json->type == TYPE_INT32)
    {
        int32_t *val = my_malloc(int32_t);
        RANDOM_ACCESS_FREAD_OR_FAIL(val, sizeof(int32_t), entity.val_ptr, section->header.filp);

        json->value.int32_val = *val;
    } else if (json->type == TYPE_BOOL)
    {
        bool *val = my_malloc(bool);
        RANDOM_ACCESS_FREAD_OR_FAIL(val, sizeof(bool), entity.val_ptr, section->header.filp);

        json->value.int32_val = *val;
    } else if (json->type == TYPE_FLOAT)
    {
        float* val = my_malloc(float);
        RANDOM_ACCESS_FREAD_OR_FAIL(val, sizeof(float), entity.val_ptr, section->header.filp);

        json->value.int32_val = *val;
    } else if (json->type == TYPE_STRING)
    {
        char *val = my_malloc_array(char, entity.val_size);
        RANDOM_ACCESS_FREAD_OR_FAIL(val, entity.val_size, entity.val_ptr, section->header.filp);

        string_ctor(&json->value.string_val, val, entity.val_size);
    }

    return OK;
}

/*
Если старая запись находится на границе, можем проверить вместимость по свободному месту в секции

Иначе проверим вместимость по размеру старой записи(сумма размеров ключа и значения)
*/
PerformStatus section_extents_update(section_extents_t *const section, const sectoff_t offset, const json_value_t *const new_json)
{
    json_value_entity_t old_entity;
    RANDOM_ACCESS_FREAD_OR_FAIL(&old_entity, sizeof(json_value_entity_t), offset, section->header.filp);

    json_value_entity_t new_entity;
    json_value_entity_ctor(&new_entity, new_json, old_entity.dad_ptr, old_entity.bro_ptr, old_entity.son_ptr);

    if (offset + sizeof(json_value_entity_t) == section->header.last_item_ptr)
    {
        if (section->header.free_space + json_value_entity_get_physical_size(&old_entity) - json_value_entity_get_physical_size(&new_entity) >= 0)
        {
            fileoff_t save_addr;
            section_header_shift_last_item_ptr(&section->header, old_entity.key_size + old_entity.val_size);
            section_extents_write(section, new_json, new_entity.dad_ptr, new_entity.bro_ptr, new_entity.son_ptr, &save_addr);

            return OK;
        }
    }
    else
    {
        if (json_value_entity_get_physical_size(&old_entity) >= json_value_entity_get_physical_size(&new_entity))
        {
            sectoff_t prev_last_item_ptr = section->header.last_item_ptr;
            sectoff_t prev_first_record_ptr = section->header.first_record_ptr;

            section->header.last_item_ptr = offset;
            section->header.first_record_ptr = old_entity.key_ptr + old_entity.key_size;

            fileoff_t save_addr;
            section_extents_write(section, new_json, new_entity.dad_ptr, new_entity.bro_ptr, new_entity.son_ptr, &save_addr);

            section->header.last_item_ptr = prev_last_item_ptr;
            section->header.first_record_ptr = prev_first_record_ptr;

            return OK;
        }
    }

    return FAILED;
}

/*
    Если нода является последней записанной записью, мы можем подвинуть указатели, чтобы не образовывались пропуски
*/
PerformStatus section_documents_delete(section_extents_t *section, sectoff_t offset)
{
    if (offset + sizeof(json_value_entity_t) == section->header.last_item_ptr)
    {
        json_value_entity_t entity;
        RANDOM_ACCESS_FREAD_OR_FAIL(&entity, sizeof(json_value_entity_t), offset, section->header.filp);

        section_header_shift_last_item_ptr(&section->header, -1 * sizeof(json_value_entity_t));
        section_header_shift_first_record_ptr(&section->header, entity.key_size + entity.val_size);
    }

    return OK;
}

static PerformStatus section_extents_write_in_item(section_extents_t *const section, const size_t data_size, const void *const data_ptr)
{
    long prev_ptr = ftell(section->header.filp);
    RANDOM_ACCESS_FWRITE_OR_FAIL(data_ptr, data_size, section->header.last_item_ptr, section->header.filp);
    section_header_shift_last_item_ptr((section_header_t *)section, data_size);
    FSEEK_OR_FAIL(section->header.filp, prev_ptr);

    return OK;
}

static PerformStatus section_extents_write_in_record(section_extents_t *const section, const size_t data_size, const void *const data_ptr, fileoff_t *const save_addr)
{
    long prev_ptr = ftell(section->header.filp);

    section_header_shift_first_record_ptr((section_header_t *)section, -1 * data_size);
    FSEEK_OR_FAIL(section->header.filp, section->header.first_record_ptr);
    FWRITE_OR_FAIL(data_ptr, data_size, section->header.filp); // Bad memory access TODO see memory alloc

    *save_addr = section->header.first_record_ptr;

    FSEEK_OR_FAIL(section->header.filp, prev_ptr);

    return OK;
}