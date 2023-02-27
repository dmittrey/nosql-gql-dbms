#include <string.h>

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"
#include "memory/section/extents.h"

static status_t sect_ext_write_itm(sect_ext_t *const, const size_t, const void *const);
static status_t sect_ext_write_rec(sect_ext_t *const, const size_t, const void *const, fileoff_t *const);

sect_ext_t *sect_ext_new()
{
    return memset(my_malloc(sect_ext_t), 0, sizeof(sect_ext_t));
}

status_t sect_ext_ctor(sect_ext_t *const section, const fileoff_t offset, FILE *const filp)
{
    section->next = NULL;
    return sect_head_ctor((sect_head_t *)section, offset, filp);
}
void sect_ext_dtor(sect_ext_t *section)
{
    sect_head_dtor((sect_head_t *)section);
}

status_t sect_ext_write(sect_ext_t *const section, const json_t *const json, entity_t *const entity, sectoff_t *const save_addr)
{
    if (section->header.free_space < sizeof(entity_t))
        return FAILED;

    *save_addr = section->header.lst_itm_ptr;

    DO_OR_FAIL(sect_ext_write_rec(section, sizeof(char) * json->key->cnt, json->key->val, &entity->key_ptr));

    if (entity->type != TYPE_OBJECT)
    {
        DO_OR_FAIL(sect_ext_write_rec(section, json_val_size(json), json_val_ptr(json), &entity->val_ptr));
    }

    DO_OR_FAIL(sect_ext_write_itm(section, sizeof(entity_t), entity));

    return OK;
}

status_t sect_ext_read(const sect_ext_t *const section, const sectoff_t sect_addr, entity_t *const o_entity, json_t *const o_json)
{
    RA_FREAD_OR_FAIL(o_entity, sizeof(entity_t), sect_head_get_fileoff(&section->header, sect_addr), section->header.filp);

    // Key parsing
    char *key = my_malloc_array(char, o_entity->key_size);
    RA_FREAD_OR_FAIL(key, o_entity->key_size, sect_head_get_fileoff(&section->header, o_entity->key_ptr), section->header.filp);

    json_ctor(o_json, o_entity->type, key, o_entity->key_size);

    if (o_json->type == TYPE_INT32)
    {
        int32_t val;
        RA_FREAD_OR_FAIL(&val, sizeof(int32_t), sect_head_get_fileoff(&section->header, o_entity->val_ptr), section->header.filp);

        o_json->value.int32_val = val;
    }
    if (o_json->type == TYPE_BOOL)
    {
        bool val;
        RA_FREAD_OR_FAIL(&val, sizeof(bool), sect_head_get_fileoff(&section->header, o_entity->val_ptr), section->header.filp);

        o_json->value.bool_val = val;
    }
    if (o_json->type == TYPE_FLOAT)
    {
        float val;
        RA_FREAD_OR_FAIL(&val, sizeof(float), sect_head_get_fileoff(&section->header, o_entity->val_ptr), section->header.filp);

        o_json->value.float_val = val;
    }
    if (o_json->type == TYPE_STRING)
    {
        char *val = my_malloc_array(char, o_entity->val_size);
        RA_FREAD_OR_FAIL(val, o_entity->val_size, sect_head_get_fileoff(&section->header, o_entity->val_ptr), section->header.filp);

        o_json->value.string_val = string_new();
        string_ctor(o_json->value.string_val, val, o_entity->val_size);
        free(val);
    }

    free(key);

    return OK;
}

/*
Если старая запись находится на границе, можем проверить вместимость по свободному месту в секции

Иначе проверим вместимость по размеру старой записи(сумма размеров ключа и значения)
*/
status_t sect_ext_update(sect_ext_t *const section, const sectoff_t offset, const json_t *const new_json)
{
    entity_t *old_entity = entity_new();
    RA_FREAD_OR_FAIL(old_entity, sizeof(entity_t), sect_head_get_fileoff(&section->header, offset), section->header.filp);

    entity_t *new_entity = entity_new();
    entity_ctor(new_entity, new_json, old_entity->fam_addr.dad_ptr, old_entity->fam_addr.bro_ptr, old_entity->fam_addr.son_ptr);

    if (section->header.free_space + entity_ph_size(old_entity) - entity_ph_size(new_entity) >= 0)
    {
        if (offset + sizeof(entity_t) == section->header.lst_itm_ptr)
        {
            section->header.lst_itm_ptr -= entity_itm_size(old_entity);
            section->header.fst_rec_ptr += entity_rec_size(old_entity);
            section->header.free_space += entity_ph_size(old_entity);

            fileoff_t save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity, &save_addr));

            entity_dtor(new_entity);
            entity_dtor(old_entity);

            return OK;
        }
        else if (entity_rec_size(new_entity) <= old_entity->rec_size)
        {
            new_entity->rec_size = old_entity->rec_size;

            sectoff_t prev_last_item_ptr = section->header.lst_itm_ptr;
            sectoff_t prev_first_record_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = offset;
            section->header.fst_rec_ptr = old_entity->key_ptr + old_entity->key_size;
            section->header.free_space += entity_ph_size(old_entity);

            fileoff_t save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity, &save_addr));

            section->header.lst_itm_ptr = prev_last_item_ptr;
            section->header.fst_rec_ptr = prev_first_record_ptr;

            entity_dtor(new_entity);
            entity_dtor(old_entity);

            return OK;
        }
    }

    entity_dtor(new_entity);
    entity_dtor(old_entity);

    return FAILED;
}

/*
    Если нода является последней записанной записью, мы можем подвинуть указатели, чтобы не образовывались пропуски
*/
status_t sect_ext_delete(sect_ext_t *const section, const sectoff_t offset)
{
    if (offset + sizeof(entity_t) == section->header.lst_itm_ptr)
    {
        entity_t *entity = entity_new();
        RA_FREAD_OR_FAIL(entity, sizeof(entity_t), sect_head_get_fileoff(&section->header, offset), section->header.filp);

        DO_OR_FAIL(sect_head_shift_lst_itm_ptr(&section->header, -1 * entity_itm_size(entity)));
        DO_OR_FAIL(sect_head_shift_fst_rec_ptr(&section->header, entity_rec_size(entity)));

        entity_dtor(entity);
    }

    return OK;
}

static status_t sect_ext_write_itm(sect_ext_t *const section, const size_t data_size, const void *const data_ptr)
{
    SAVE_FILP(section->header.filp, {
        RA_FWRITE_OR_FAIL(data_ptr, data_size, sect_head_get_fileoff(&section->header, section->header.lst_itm_ptr), section->header.filp);
        DO_OR_FAIL(sect_head_shift_lst_itm_ptr((sect_head_t *)section, data_size));
    });

    return OK;
}

static status_t sect_ext_write_rec(sect_ext_t *const section, const size_t data_size, const void *const data_ptr, fileoff_t *const save_addr)
{
    SAVE_FILP(section->header.filp, {
        DO_OR_FAIL(sect_head_shift_fst_rec_ptr((sect_head_t *)section, -data_size));
        RA_FWRITE_OR_FAIL(data_ptr, data_size, sect_head_get_fileoff(&section->header, section->header.fst_rec_ptr), section->header.filp);
        *save_addr = section->header.fst_rec_ptr;
    });

    return OK;
}

status_t sect_ext_sync(sect_ext_t *const section)
{
    return sect_head_sync(&section->header);
}

status_t sect_ext_add_next(sect_ext_t *const extents);