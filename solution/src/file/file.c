#include "memory/file/file.h"

static sect_ext_t *get_sect_ext(const file_t *const file, fileoff_t fileoff);
static sect_ext_t *find_sect_ext(const file_t *const file, const size_t entity_size);

static sect_type_t *find_sect_type(const file_t *const file, const size_t type_size);

file_t *file_new()
{
    return my_malloc(file_t);
}

void file_ctor(file_t *const file, FILE *const filp)
{
    file->header.lst_sect_ptr = sizeof(file_head_t);
    file->f_extent = NULL;
    file->f_types = NULL;
    file->filp = filp;
}
void file_dtor(file_t *file)
{
    sect_ext_t *cur_extent = file->f_extent;
    sect_ext_t *next_extent = NULL;
    while (cur_extent != NULL)
    {
        next_extent = cur_extent->next;
        sect_ext_dtor(cur_extent);
        cur_extent = next_extent;
    }

    sect_type_t *cur_type = file->f_types;
    sect_type_t *next_type = NULL;
    while (cur_type != NULL)
    {
        next_type = cur_type->next;
        sect_type_dtor(cur_type);
        cur_type = next_type;
    }

    fclose(file->filp);

    free(file);
}

status_t file_add_type(file_t *const file, const type_t *const type)
{
    sect_type_t *types = find_sect_type(file, type_ph_sz(type));
    if (types == NULL)
    {
        types = sect_type_new();
        DO_OR_FAIL(file_add_sect_types(file, types));
    }

    sectoff_t wrt_adr;
    return sect_type_write(types, type, &wrt_adr);
}

/*
Идем по секциям типов, если нашли тип, то удалили его по отступу
*/
status_t file_delete_type(file_t *const file, const string_t *const name)
{
    sect_type_t *del_types = NULL;
    type_t *del_type = type_new();

    if (file_find_type(file, name, del_type, del_types) == OK)
    {
        sect_type_delete(del_types, del_type->soff_ptr);
        return OK;
    }
    else
    {
        return FAILED;
    }
}
status_t file_find_type(file_t *const file, const string_t *const name, type_t *const o_type, sect_type_t *o_types)
{
    type_t *f_type = type_new();

    type_t *zr_type = type_new();

    sect_type_t *types = file->f_types;
    while (types != NULL)
    {
        DO_OR_FAIL(sect_type_find(types, name, f_type));
        // Нашли
        if (memcpy(f_type, zr_type, sizeof(type_t)) != 0)
        {
            o_types = types;
            type_cpy(o_type, f_type);
            break;
        }

        types = types->next;
    }

    type_dtor(f_type);
    type_dtor(zr_type);

    if (o_types == NULL)
        return FAILED;
    else
        return OK;
}

/*
cur -> bro -> son
*/
status_t file_write(file_t *const file, const json_t *const json, fileoff_t dad_fileoff, fileoff_t *const write_addr)
{
    ENTITY_INIT(entity, json, dad_fileoff, 0, 0);

    sect_ext_t *extents = find_sect_ext(file, entity_ph_size(entity));
    if (extents == NULL)
    {
        extents = sect_ext_new();
        DO_OR_FAIL(file_add_sect_ext(file, extents));
    }

    size_t entity_size = entity_ph_size(entity);
    size_t a = entity_itm_size(entity);
    sect_ext_write(extents, json, entity, write_addr);

    *write_addr += extents->header.sect_off;

    fileoff_t bro_offset = 0;
    if (json->bro != NULL)
    {
        DO_OR_FAIL(file_write(file, json->bro, dad_fileoff, &bro_offset));
        entity->fam_addr.bro_ptr = bro_offset;
    }

    fileoff_t son_offset = 0;
    if (json->son != NULL)
    {
        DO_OR_FAIL(file_write(file, json->son, *write_addr, &son_offset));
        entity->fam_addr.son_ptr = son_offset;
    }

    if (bro_offset)
    {
        RA_FWRITE_OR_FAIL(&bro_offset, sizeof(bro_offset), *write_addr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, bro_ptr), file->filp);
    }
    if (son_offset)
    {
        RA_FWRITE_OR_FAIL(&son_offset, sizeof(son_offset), *write_addr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
    }

    entity_dtor(entity);

    return OK;
}
/*
1) Прочитать секцию с нодой
2) Прочитать ноду по секционному смещению
3) Если есть брат, запуск от брата
4) Прокинуть ссылку на брата
5) Если есть сын, запуск от сына
6) Прокинуть ссылку на сына
*/
status_t file_read(file_t *const file, const fileoff_t fileoff, json_t *const ret_json)
{
    sect_ext_t *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
        return FAILED;

    entity_t *ret_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), ret_entity, ret_json));

    if (ret_entity->fam_addr.bro_ptr != 0)
    {
        json_t *bro_json = json_new();
        DO_OR_FAIL(file_read(file, ret_entity->fam_addr.bro_ptr, bro_json));
        json_add_bro(ret_json, bro_json);
    }

    if (ret_entity->fam_addr.son_ptr != 0)
    {
        json_t *son_json = json_new();
        DO_OR_FAIL(file_read(file, ret_entity->fam_addr.son_ptr, son_json));
        json_add_son(ret_json, son_json);
    }

    entity_dtor(ret_entity);
    return OK;
}

/*
1) Считали старую ноду
2) Сравнили ноду, если надо, то обновили
3) Обновили поддерево
*/
status_t file_update(file_t *const file, const fileoff_t fileoff, const json_t *const new_json, const fileoff_t dad_ptr, bool is_bro_upd, fileoff_t *cur_fileoff)
{
    sect_ext_t *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
    {
        return FAILED;
    }

    json_t *old_json = json_new();
    entity_t *old_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), old_entity, old_json));

    *cur_fileoff = fileoff;
    ENTITY_INIT(new_entity, new_json, dad_ptr, 0, 0);

    if (json_cmp(old_json, new_json) != 0)
    {
        /*
            - Если не можем обновить, то удалим сущность из секции
            - Далее добавим сущность в первую свободную секцию
            - Обновим дерево под ней
        */
        if (sect_ext_update(extents, sect_head_get_sectoff(&extents->header, fileoff), new_json, new_entity) == FAILED)
        {
            DO_OR_FAIL(sect_ext_delete(extents, sect_head_get_sectoff(&extents->header, fileoff), NULL));

            sect_ext_t *empty_sect_ext = find_sect_ext(file, entity_ph_size(new_entity));
            if (empty_sect_ext == NULL)
            {
                empty_sect_ext = sect_ext_new();
                DO_OR_FAIL(file_add_sect_ext(file, empty_sect_ext));
            }

            sectoff_t save_addr;
            DO_OR_FAIL(sect_ext_write(empty_sect_ext, new_json, new_entity, &save_addr));

            *cur_fileoff = sect_head_get_fileoff(&empty_sect_ext->header, save_addr);
        }
    }

    fileoff_t bro_ptr = 0;
    if (is_bro_upd && new_json->bro != NULL)
    {
        DO_OR_FAIL(file_update(file, old_entity->fam_addr.bro_ptr, new_json->bro, *cur_fileoff, true, &bro_ptr));
    }

    fileoff_t son_ptr = 0;
    if (new_json->son != NULL)
    {
        DO_OR_FAIL(file_update(file, old_entity->fam_addr.son_ptr, new_json->son, *cur_fileoff, true, &son_ptr));
    }

    if (bro_ptr)
    {
        RA_FWRITE_OR_FAIL(&bro_ptr, sizeof(bro_ptr), *cur_fileoff + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, bro_ptr), file->filp);
    }
    if (son_ptr)
    {
        RA_FWRITE_OR_FAIL(&son_ptr, sizeof(son_ptr), *cur_fileoff + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
    }

    json_dtor(old_json);
    entity_dtor(old_entity);
    entity_dtor(new_entity);
    return OK;
}

/*
1) Нашли секцию с удаляемой нодой, если не нашли то FAILED
2) Удалили текущую ноду и вернули ее сущность
3) Удалили дерево под ней
4) Поменяли ссылку с отца на сына
*/
status_t file_delete(file_t *const file, const fileoff_t fileoff, bool is_root)
{
    // Find root section
    sect_ext_t *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
    {
        return FAILED;
    }

    // Delete and read del root
    entity_t *const del_entity = entity_new();
    DO_OR_FAIL(sect_ext_delete(extents, sect_head_get_sectoff(&extents->header, fileoff), del_entity));

    if (!is_root && del_entity->fam_addr.bro_ptr != 0)
    {
        DO_OR_FAIL(file_delete(file, del_entity->fam_addr.bro_ptr, false));
    }

    // Delete recursively under tree
    if (del_entity->fam_addr.son_ptr != 0)
    {
        DO_OR_FAIL(file_delete(file, del_entity->fam_addr.son_ptr, false));
    }

    // If dad exist change dad -> son ptr
    if (is_root && del_entity->fam_addr.dad_ptr != 0)
    {
        // Change dad -> son to bro_ptr
        if (del_entity->fam_addr.bro_ptr != 0)
        {
            RA_FWRITE_OR_FAIL(&del_entity->fam_addr.bro_ptr, sizeof(fileoff_t), del_entity->fam_addr.dad_ptr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
        }
        // Change dad -> son to 0
        else
        {
            fileoff_t temp_zero = 0;
            RA_FWRITE_OR_FAIL(&temp_zero, sizeof(fileoff_t), del_entity->fam_addr.dad_ptr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
        }
    }

    entity_dtor(del_entity);
    return OK;
}

/*
1) Получили массив json'ов которые удовлетворяют хотя бы одному условию
2) Итерируемся по каждому
    - Прочитали по ссылке на родителя
    - Прогнали по всем условиям
    - Если соответствует то сохранили в o_obj_col
*/
status_t file_find(file_t *const file, const query_t *const query, list_json_t *const o_obj_col)
{
    list_json_t *temp = list_json_t_new();
    DO_OR_FAIL(sect_ext_find(file->f_extent, query, temp));

    while (temp->head != NULL)
    {
        json_t *dad_json = json_new();
        DO_OR_FAIL(file_read(file, temp->head->entity->fam_addr.dad_ptr, dad_json));

        if (query_check_and(query, dad_json))
        {
            if (!list_json_t_add_lk_set(o_obj_col, dad_json))
            {
                json_dtor_with_bro(dad_json);
            }
        }
        else
        {
            json_dtor_with_bro(dad_json);
        }

        list_json_t_del_fst(temp);
    }

    list_json_t_dtor(temp);

    return OK;
}

/*
Add empty extents section to end of file
*/
status_t file_add_sect_ext(file_t *const file, sect_ext_t *new_extents)
{
    DO_OR_FAIL(sect_ext_ctor(new_extents, file->header.lst_sect_ptr, file->filp));
    file->header.lst_sect_ptr += SECTION_SIZE;
    if (file->f_extent == NULL)
    {
        file->f_extent = new_extents;
        return OK;
    }

    sect_ext_t *cur = file->f_extent;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = new_extents;
    cur->header.next_ptr = new_extents->header.sect_off;
    return sect_ext_sync(cur);
}

/*
Add empty types section to end of file
*/
status_t file_add_sect_types(file_t *const file, sect_type_t *new_types)
{
    DO_OR_FAIL(sect_type_ctor(new_types, file->header.lst_sect_ptr, file->filp));
    file->header.lst_sect_ptr += SECTION_SIZE;
    if (file->f_types == NULL)
    {
        file->f_types = new_types;
        return OK;
    }

    sect_type_t *cur = file->f_types;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = new_types;
    cur->header.next_ptr = new_types->header.sect_off;
    return sect_types_sync(cur);
}

static sect_ext_t *get_sect_ext(const file_t *const file, fileoff_t fileoff)
{
    sect_ext_t *r_sect_ext = file->f_extent;

    while (r_sect_ext != NULL)
    {
        if (r_sect_ext->header.sect_off <= fileoff && r_sect_ext->header.sect_off + SECTION_SIZE > fileoff)
        {
            break;
        }

        r_sect_ext = r_sect_ext->next;
    }

    return r_sect_ext;
}

static sect_ext_t *find_sect_ext(const file_t *const file, const size_t entity_size)
{
    sect_ext_t *cur_ext = file->f_extent;
    while (cur_ext != NULL && cur_ext->header.free_space < entity_size)
    {
        cur_ext = cur_ext->next;
    }

    return cur_ext;
}

static sect_type_t *find_sect_type(const file_t *const file, const size_t type_size)
{
    if (file->f_types == NULL)
        return NULL;

    sect_type_t *cur_types = file->f_types;
    while (cur_types != NULL && cur_types->header.free_space < type_size)
    {
        cur_types = cur_types->next;
    }

    return cur_types;
}