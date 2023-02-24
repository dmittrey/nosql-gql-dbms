#include <string.h>

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"
#include "memory/section/extents.h"

#include "physical/json/entity.h"

static status_t sect_ext_write_itm(sect_ext_t *const, const size_t, const void *const);
static status_t sect_ext_write_rec(sect_ext_t *const, const size_t, const void *const, fileoff_t *const);

sect_ext_t *sect_ext_new()
{
    return memset(my_malloc(sect_ext_t), 0, sizeof(sect_ext_t));
}

status_t sect_ext_ctor(sect_ext_t *const section, const fileoff_t offset, FILE *const filp)
{
    return sect_head_ctor((sect_head_t *)section, offset, filp);
}
void sect_ext_dtor(sect_ext_t *section)
{
    sect_head_dtor((sect_head_t *)section);
}

status_t sect_ext_write(sect_ext_t *const section, const json_t *const json, const fileoff_t dad_addr, const fileoff_t bro_addr, const fileoff_t son_addr, fileoff_t *const save_addr)
{
    entity_t entity;
    entity_ctor(&entity, json, dad_addr, bro_addr, son_addr);

    if (section->header.free_space < sizeof(entity_t))
        return FAILED;

    *save_addr = section->header.lst_itm_ptr;

    DO_OR_FAIL(sect_ext_write_rec(section, sizeof(char) * json->key->cnt, json->key->val, &entity.key_ptr));

    if (entity.type != TYPE_OBJECT){
        DO_OR_FAIL(sect_ext_write_rec(section, json_val_size(json), json_val_ptr(json), &entity.val_ptr));
    }

    DO_OR_FAIL(sect_ext_write_itm(section, sizeof(entity_t), &entity));
    
    return OK;
}

status_t sect_ext_read(const sect_ext_t *const section, const sectoff_t sect_addr, json_t *const o_json)
{
    entity_t entity;
    RA_FREAD_OR_FAIL(&entity, sizeof(entity_t), sect_addr, section->header.filp);

    // Key parsing
    char *key = my_malloc_array(char, entity.key_size);
    RA_FREAD_OR_FAIL(key, entity.key_size, entity.key_ptr, section->header.filp);

    json_ctor(o_json, entity.type, key, entity.key_size);

    if (o_json->type == TYPE_INT32)
    {
        int32_t *val = my_malloc(int32_t);
        RA_FREAD_OR_FAIL(val, sizeof(int32_t), entity.val_ptr, section->header.filp);

        o_json->value.int32_val = *val;
    }
    if (o_json->type == TYPE_BOOL)
    {
        bool *val = my_malloc(bool);
        RA_FREAD_OR_FAIL(val, sizeof(bool), entity.val_ptr, section->header.filp);

        o_json->value.bool_val = *val;
    }
    if (o_json->type == TYPE_FLOAT)
    {
        float *val = my_malloc(float);
        RA_FREAD_OR_FAIL(val, sizeof(float), entity.val_ptr, section->header.filp);

        o_json->value.float_val = *val;
    }
    if (o_json->type == TYPE_STRING)
    {
        char *val = my_malloc_array(char, entity.val_size);
        RA_FREAD_OR_FAIL(val, entity.val_size, entity.val_ptr, section->header.filp);

        string_ctor(o_json->value.string_val, val, entity.val_size);
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
    RA_FREAD_OR_FAIL(old_entity, sizeof(entity_t), offset, section->header.filp);

    entity_t *new_entity = entity_new();
    entity_ctor(new_entity, new_json, old_entity->dad_ptr, old_entity->bro_ptr, old_entity->son_ptr);

    if (offset + sizeof(entity_t) == section->header.lst_itm_ptr)
    {
        if (section->header.free_space + sizeof(entity_t) - sizeof(entity_t) >= 0)
        {
            fileoff_t save_addr;
            DO_OR_FAIL(
                sect_head_shift_lst_itm_ptr(&section->header, old_entity->key_size + old_entity->val_size));
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity->dad_ptr, new_entity->bro_ptr, new_entity->son_ptr, &save_addr));

            return OK;
        }
    }
    else
    {
        if (sizeof(entity_t) >= sizeof(entity_t))
        {
            sectoff_t prev_last_item_ptr = section->header.lst_itm_ptr;
            sectoff_t prev_first_record_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = offset;
            section->header.fst_rec_ptr = old_entity->key_ptr + old_entity->key_size;

            fileoff_t save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity->dad_ptr, new_entity->bro_ptr, new_entity->son_ptr, &save_addr));

            section->header.lst_itm_ptr = prev_last_item_ptr;
            section->header.fst_rec_ptr = prev_first_record_ptr;

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
        RA_FREAD_OR_FAIL(entity, sizeof(entity_t), offset, section->header.filp);

        DO_OR_FAIL(sect_head_shift_lst_itm_ptr(&section->header, -1 * sizeof(entity_t)));
        DO_OR_FAIL(sect_head_shift_fst_rec_ptr(&section->header, entity->key_size + entity->val_size));

        entity_dtor(entity);
    }

    return OK;
}

static status_t sect_ext_write_itm(sect_ext_t *const section, const size_t data_size, const void *const data_ptr)
{
    SAVE_FILP(section->header.filp, {
        RA_FWRITE_OR_FAIL(data_ptr, data_size, section->header.lst_itm_ptr, section->header.filp);
        DO_OR_FAIL(sect_head_shift_lst_itm_ptr((sect_head_t *)section, data_size));
    });

    return OK;
}

static status_t sect_ext_write_rec(sect_ext_t *const section, const size_t data_size, const void *const data_ptr, fileoff_t *const save_addr)
{
    long prev_ptr = ftell(section->header.filp);

    DO_OR_FAIL(sect_head_shift_fst_rec_ptr((sect_head_t *)section, -data_size));
    FSEEK_OR_FAIL(section->header.filp, section->header.fst_rec_ptr);
    FWRITE_OR_FAIL(data_ptr, data_size, section->header.filp); // Bad memory access TODO see memory alloc

    *save_addr = section->header.fst_rec_ptr;

    return OK;
}

status_t sect_ext_sync(sect_ext_t *const section)
{
    return sect_head_sync(&section->header);
}