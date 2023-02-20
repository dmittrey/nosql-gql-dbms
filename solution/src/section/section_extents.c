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

PerformStatus section_extents_write(section_extents_t *const section, const json_value_t *const json, const fileoff_t parent_json_addr, const fileoff_t bro_json_addr, fileoff_t *const save_json_addr)
{
    // size_t json_size = json_value_get_serialization_size(json);

    // if (section->header.free_space >= json_size)
    // {
    fileoff_t json_val_ptr = 0;
    fileoff_t json_key_ptr = 0;

    fileoff_t save_addr = section->header.last_item_ptr;

    json_value_entity *entity = json_value_entity_new();
    json_value_entity_ctor(entity, json);
    entity->dad_ptr = parent_json_addr;
    entity->bro_ptr = bro_json_addr;

    section_header_shift_last_item_ptr((section_header_t *)section, sizeof(json_value_entity));

    section_extents_write_in_record(section, sizeof(char) * json->key.count, json->key.val, &json_key_ptr);
    section_extents_write_in_record(section, json_value_get_val_size(json), json_value_get_val_ptr(json), &json_val_ptr);

    if (json->type == TYPE_OBJECT)
    {
        fileoff_t son_save_addr = NULL;

        struct json_value_t *cur_son = json->son;
        while (cur_son != NULL)
        {
            /*
            Записывать будем в обратном порядке
            3 -> 2 -> 1
            parent -> 3
            3 -> parent
            */
            if (cur_son->bro == NULL)
            {
                section_extents_write(section, cur_son, save_addr, son_save_addr, &son_save_addr);
            }
            else
            {
                section_extents_write(section, cur_son, NULL, son_save_addr, &son_save_addr);
            }
            cur_son = cur_son->bro;
        }

        entity->son_ptr = son_save_addr;
    }

    RANDOM_ACCESS_FWRITE_OR_FAIL(entity, sizeof(json_value_entity), save_addr, section->header.filp);

    *save_json_addr = save_addr;
    // }

    return OK;
}

PerformStatus section_extents_read(const section_extents_t *const section, const sectoff_t offset, json_value_t *const json)
{
    return OK;
}

PerformStatus section_extents_update(section_extents_t *const section, const sectoff_t offset, const json_value_t *const new_json)
{
    return OK;
}

PerformStatus section_documents_delete(section_extents_t *section, sectoff_t offset)
{
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