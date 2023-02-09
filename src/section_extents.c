#include "utils.h"
#include "table.h"

#include "section/section_header.h"
#include "section/section_extents.h"

static PerformStatus section_extents_write_in_item(section_extents_t *, size_t, void *);
static PerformStatus section_extents_write_in_record(section_extents_t *, size_t, void *, fileoff_t *);

section_extents_t *section_documents_new()
{
    return my_malloc(section_extents_t);
}

void section_extents_ctor(section_extents_t *section, fileoff_t offset, FILE *filp)
{
    section_header_ctor((section_header_t *)section, offset, filp);
}
void section_extents_dtor(section_extents_t *section)
{
    section_page_dtor((section_header_t *)section);
}

PerformStatus section_extents_sync(section_extents_t *section)
{
    return section_page_sync((section_header_t *)section);
}

/*
1) Сериализовать согласно физическому представлению
2) Записать Items
3) Записать Records
*/
PerformStatus section_extents_write(section_extents_t *section, json_value_t *json, fileoff_t *save_addr)
{
    if (section->header.free_space <= json_value_get_item_size(json) + json_value_get_record_size(json))
    {
        long prev_ptr = ftell(section->header.filp);
        FSEEK_OR_FAIL(section->header.filp, section->header.last_item_ptr);
        *save_addr = section->header.last_item_ptr;

        FWRITE_OR_FAIL(&json->object.attributes_count, sizeof(uint64_t), section->header.filp);
        for (size_t i = 0; i < json->object.attributes_count; i++)
        {
            // Write key length
            section_extents_write_in_item(section, sizeof(uint64_t), &json->object.attributes[i]->key.count);

            // Write key value and ptr
            fileoff_t key_val_ptr = 0;
            fileoff_t obj_val_ptr = 0;
            section_extents_write_in_record(section, string_get_size(json->object.attributes[i]->key), json->object.attributes[i]->key.val, &key_val_ptr);
            section_extents_write(section, json->object.attributes[i]->value, &obj_val_ptr);

            // Write key name ptr and value ptr
            section_extents_write_in_item(section, sizeof(uint64_t), key_val_ptr);
            section_extents_write_in_item(section, sizeof(uint64_t), obj_val_ptr);
        }

        FWRITE_OR_FAIL(&json->type, sizeof(uint64_t), section->header.filp);

        fileoff_t json_val_ptr = 0;
        if (json->type == TYPE_STRING)
        {
            section_extents_write_in_record(section, string_get_size(json->value.string_val), json->value.string_val.val, &json_val_ptr);
        }
        else
        {
            section_extents_write_in_record(section, sizeof(json->value), &json->value, &json_val_ptr);
        }
        section_extents_write_in_item(section, sizeof(uint64_t), json_val_ptr);

        return OK;
    }

    return FAILED;
}
PerformStatus section_documents_read(section_extents_t *, json_value_t *);
PerformStatus section_documents_update(section_extents_t *, json_value_t *);
PerformStatus section_documents_delete(section_extents_t *, json_value_t *);

static PerformStatus section_extents_write_in_item(section_extents_t *section, size_t data_size, void *data_ptr)
{
    long prev_ptr = ftell(section->header.filp);

    FSEEK_OR_FAIL(section->header.filp, section->header.last_item_ptr);
    FWRITE_OR_FAIL(section->header.filp, data_size, data_ptr);
    section_page_shift_last_item_ptr((section_header_t *)section, data_size);

    FSEEK_OR_FAIL(section->header.filp, prev_ptr);
}

static PerformStatus section_extents_write_in_record(section_extents_t *section, size_t data_size, void *data_ptr, fileoff_t *save_addr)
{
    long prev_ptr = ftell(section->header.filp);

    section_header_shift_first_record_ptr((section_header_t *)section, -1 * data_size);
    FSEEK_OR_FAIL(section->header.filp, section->header.first_record_ptr);
    FWRITE_OR_FAIL(section->header.filp, data_size, data_ptr);

    *save_addr = section->header.first_record_ptr;

    FSEEK_OR_FAIL(section->header.filp, prev_ptr);
}