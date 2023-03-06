#include <string.h>

#include "utils.h"
#include "table.h"

#include "memory/json/json_col.h"

#include "memory/section/header.h"
#include "memory/section/extents.h"

#include "physical/section/header.h"

static status_t reduce_lst_itm_ptr_emt(sect_ext_t *section);
static status_t reduce_fst_rec_ptr_emt(sect_ext_t *section);
static status_t sect_ext_load(const sect_ext_t *const section, json_col_t *const collection);

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

    DO_OR_FAIL(sect_head_shift_fst_rec_ptr((sect_head_t *)section, -1 * entity_rec_size(entity)));
    DO_OR_FAIL(sect_ext_wrt_rec(section, section->header.fst_rec_ptr + entity->val_size, json->key->val, json->key->cnt));
    entity->key_ptr = section->header.fst_rec_ptr + entity->val_size;

    DO_OR_FAIL(sect_ext_wrt_rec(section, section->header.fst_rec_ptr, json_val_ptr(json), json_val_size(json)));
    entity->val_ptr = section->header.fst_rec_ptr;

    DO_OR_FAIL(sect_ext_wrt_itm(section, section->header.lst_itm_ptr, entity));
    DO_OR_FAIL(sect_head_shift_lst_itm_ptr((sect_head_t *)section, sizeof(entity_t)));

    return OK;
}

status_t sect_ext_read(const sect_ext_t *const section, const sectoff_t sectoff, entity_t *const o_entity, json_t *const o_json)
{
    DO_OR_FAIL(sect_ext_rd_itm(section, sectoff, o_entity));

    // Key parsing
    char *key = my_malloc_array(char, o_entity->key_size);
    DO_OR_FAIL(sect_ext_rd_rec(section, o_entity->key_ptr, o_entity->key_size, key));

    json_ctor(o_json, o_entity->type, key, o_entity->key_size);
    free(key);

    if (o_json->type == TYPE_STRING)
    {
        char *val = my_malloc_array(char, o_entity->val_size);
        DO_OR_FAIL(sect_ext_rd_rec(section, o_entity->val_ptr, o_entity->val_size, val));

        o_json->value.string_val = string_new();
        string_ctor(o_json->value.string_val, val, o_entity->val_size);
        free(val);
    }
    else
    {
        DO_OR_FAIL(sect_ext_rd_rec(section, o_entity->val_ptr, o_entity->val_size, json_val_ptr(o_json)));
    }

    return OK;
}

/*
Если старая запись находится на границе, можем проверить вместимость по свободному месту в секции

Иначе проверим вместимость по размеру старой записи(сумма размеров ключа и значения)
*/
status_t sect_ext_update(sect_ext_t *const section, const sectoff_t sectoff, const json_t *const new_json, entity_t *const new_entity)
{
    entity_t *old_entity = entity_new();
    DO_OR_FAIL(sect_ext_rd_itm(section, sectoff, old_entity));

    if (new_entity->rec_size < old_entity->rec_size)
    {
        new_entity->rec_size = old_entity->rec_size;
    }

    // If record is boundary
    if (old_entity->key_ptr - old_entity->val_size == section->header.fst_rec_ptr)
    {
        // If we can insert
        if (section->header.free_space + old_entity->rec_size >= new_entity->rec_size)
        {
            sectoff_t prev_lst_itm_ptr = section->header.lst_itm_ptr;
            sectoff_t prev_fst_rec_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = sectoff;
            section->header.fst_rec_ptr += old_entity->rec_size;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            sectoff_t save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity, &save_addr));

            section->header.lst_itm_ptr = prev_lst_itm_ptr;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            entity_dtor(old_entity);
            return sect_head_sync(&section->header);
        }
    }
    // If record is inner
    else
    {
        // If we cannot insert record in gap
        if (new_entity->rec_size > old_entity->rec_size)
        {
            // If we can insert record in free space
            if (sectoff + entity_itm_size(old_entity) == section->header.lst_itm_ptr &&
                section->header.free_space >= entity_rec_size(new_entity))
            {
                sect_ext_delete(section, sectoff, NULL);

                sectoff_t prev_lst_itm_ptr = section->header.lst_itm_ptr;

                section->header.lst_itm_ptr = sectoff;
                section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

                sectoff_t save_addr;
                DO_OR_FAIL(
                    sect_ext_write(section, new_json, new_entity, &save_addr));

                section->header.lst_itm_ptr = prev_lst_itm_ptr;
                section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

                entity_dtor(old_entity);
                return sect_head_sync(&section->header);
            }
        }
        // If we can insert record in gap
        else
        {
            sectoff_t prev_lst_itm_ptr = section->header.lst_itm_ptr;
            sectoff_t prev_fst_rec_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = sectoff;
            section->header.fst_rec_ptr = old_entity->key_ptr + old_entity->key_size;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            sectoff_t save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_json, new_entity, &save_addr));

            section->header.lst_itm_ptr = prev_lst_itm_ptr;
            section->header.fst_rec_ptr = prev_fst_rec_ptr;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            entity_dtor(old_entity);
            return OK;
        }
    }

    entity_dtor(old_entity);
    return FAILED;
}

/*
    - Возвращаем старую сущность
    - Обнуляем поля сущности
    - Если нода стоит на границе, двигаем указатели
    - В цикле проверяем ноду на границе и если 0 то двигаем дальше
*/
status_t sect_ext_delete(sect_ext_t *const section, const sectoff_t sectoff, entity_t *o_entity)
{
    entity_t *const del_entity = entity_new();
    sect_ext_rd_itm(section, sectoff, del_entity);

    if (sectoff + entity_itm_size(del_entity) == section->header.lst_itm_ptr)
    {
        DO_OR_FAIL(sect_ext_rd_itm(section, sectoff, del_entity));

        DO_OR_FAIL(sect_head_shift_lst_itm_ptr(&section->header, -1 * entity_itm_size(del_entity)));
        DO_OR_FAIL(sect_head_shift_fst_rec_ptr(&section->header, entity_rec_size(del_entity)));
    }

    // Save old entity
    if (o_entity)
    {
        memcpy(o_entity, del_entity, sizeof(entity_t));
    }

    // Set null rec fields
    void *temp_zero = memset(my_malloc_array(char, entity_rec_size(del_entity)), 0, entity_rec_size(del_entity));
    if (del_entity->type == TYPE_OBJECT)
    {
        sect_ext_wrt_rec(section, del_entity->key_ptr, temp_zero, entity_rec_size(del_entity));
    }
    else
    {
        sect_ext_wrt_rec(section, del_entity->val_ptr, temp_zero, entity_rec_size(del_entity));
    }
    free(temp_zero);

    // Set null entity fields
    sect_ext_wrt_itm(section, sectoff, entity_clear(del_entity));

    // Shift border by null items
    reduce_lst_itm_ptr_emt(section);
    reduce_fst_rec_ptr_emt(section);

    entity_dtor(del_entity);

    return OK;
}

status_t sect_ext_find(sect_ext_t *const section, const query_t *const query, json_col_t *const o_json_col)
{
    DO_OR_FAIL(sect_ext_load(section, o_json_col));

    json_t *cur = o_json_col->f_json;
    json_t *next = cur->next;

    // Доходим до первого проверенного cur
    while (!query_check_or(query, cur))
    {
        o_json_col->f_json = o_json_col->f_json->next;
    }

    while (next != NULL)
    {
        // Если не подошел, то сдвинем next
        if (!query_check_or(query, next))
        {
            json_del_nxt(cur);
        }
        // Если подошел, то next "проверенный", двигаем оба
        else
        {
            cur = next;
        }
        
        next = cur->next;
    }

    return OK;
}

status_t sect_ext_wrt_itm(sect_ext_t *const section, const sectoff_t sectoff, const entity_t *const entity)
{
    SAVE_FILP(section->header.filp, {
        RA_FWRITE_OR_FAIL(entity, sizeof(entity_t), sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}
status_t sect_ext_rd_itm(const sect_ext_t *const section, const sectoff_t sectoff, entity_t *const o_entity)
{
    SAVE_FILP(section->header.filp, {
        RA_FREAD_OR_FAIL(o_entity, sizeof(entity_t), sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}

status_t sect_ext_wrt_rec(sect_ext_t *const section, const sectoff_t sectoff, const void *const val, const size_t size)
{
    if (size != 0)
    {
        SAVE_FILP(section->header.filp, {
            RA_FWRITE_OR_FAIL(val, size, sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
        });
    }

    return OK;
}
status_t sect_ext_rd_rec(const sect_ext_t *const section, const sectoff_t sectoff, const size_t size, void *const o_val)
{
    if (size != 0)
    {
        SAVE_FILP(section->header.filp, {
            RA_FREAD_OR_FAIL(o_val, size, sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
        });
    }

    return OK;
}

status_t sect_ext_sync(sect_ext_t *const section)
{
    return sect_head_sync(&section->header);
}

status_t sect_ext_add_next(sect_ext_t *const section, sect_ext_t *const new_section)
{
    sect_ext_t *cur = section;
    if (cur == NULL)
    {
        memcpy(section, new_section, sizeof(sect_ext_t));
    }

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    cur->next = new_section;

    return OK;
}

static status_t sect_ext_load(const sect_ext_t *const section, json_col_t *const collection)
{
    for (size_t i = sizeof(sect_head_entity_t); i < section->header.lst_itm_ptr; i += sizeof(entity_t))
    {
        json_t *o_json = json_new();
        entity_t *o_entity = entity_new();
        DO_OR_FAIL(sect_ext_read(section, i, o_entity, o_json));
        entity_dtor(o_entity);

        json_col_add(collection, o_json);
    }

    return OK;
}

static status_t reduce_lst_itm_ptr_emt(sect_ext_t *section)
{
    entity_t *lst_entity = entity_new();
    entity_t *rd_entity = entity_new();

    sect_ext_rd_itm(section, section->header.lst_itm_ptr - sizeof(entity_t), rd_entity);

    while (section->header.lst_itm_ptr != sizeof(sect_head_entity_t) && entity_cmp(lst_entity, rd_entity) == 0)
    {
        section->header.lst_itm_ptr -= sizeof(entity_t);
        section->header.free_space += sizeof(entity_t);

        sect_ext_rd_itm(section, section->header.lst_itm_ptr - sizeof(entity_t), rd_entity);
    }

    entity_dtor(lst_entity);
    entity_dtor(rd_entity);

    return sect_head_sync((sect_head_t *)section);
}
static status_t reduce_fst_rec_ptr_emt(sect_ext_t *section)
{
    char chr;
    while (true)
    {
        SAVE_FILP(section->header.filp, {
            RA_FREAD_OR_FAIL(&chr, sizeof(char), sect_head_get_fileoff(&section->header, section->header.fst_rec_ptr), section->header.filp);
        });

        if (section->header.fst_rec_ptr == SECTION_SIZE || chr != 0)
            break;

        section->header.fst_rec_ptr += 1;
        section->header.free_space += 1;
    }

    return sect_head_sync((sect_head_t *)section);
}