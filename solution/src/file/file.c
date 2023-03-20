#include "section/types.h"
#include "section/types_p.h"
#include "section/extents.h"
#include "section/extents_p.h"

#include "file/file.h"
#include "file/file_p.h"

#include "file/header.h"
#include "file/header_p.h"

File *file_new()
{
    return my_calloc(File);
}

Status file_ctor(File *const file, FILE *const filp)
{
    file->f_extent = list_Sect_ext_new();
    file->f_types = list_Sect_types_new();

    return file_head_ctor((File_head *)file, filp);
}
Status file_dtor(File *file)
{
    list_Sect_ext_dtor(file->f_extent);
    list_Sect_types_dtor(file->f_types);

    return file_head_dtor((File_head *)file);
}

Status file_add_type(File *const file, const Type *const type, Fileoff *wrt_foff)
{
    // Insert in cur section types until successful
    Sectoff wrt_soff;
    Sect_types *cur_sect = file->f_types->head;
    while (cur_sect != NULL && sect_types_write(cur_sect, type, &wrt_soff) == FAILED)
    {
        cur_sect = cur_sect->next;
    }

    // If we haven't free section, we add new and insert
    if (cur_sect == NULL)
    {
        cur_sect = sect_types_new();
        DO_OR_FAIL(file_add_sect_types(file, cur_sect));
        DO_OR_FAIL(sect_types_write(cur_sect, type, &wrt_soff));
    }

    // Convert soff write adr to foff
    *wrt_foff = sect_head_get_fileoff((Sect_head *)cur_sect, wrt_soff);

    // Sync header
    return file_head_sync((File_head *)file);
}

Status file_delete_type(File *const file, const String *const name)
{
    Sectoff del_soff;
    Sect_types *del_sect;
    Type *del_type = type_new();

    // Find type soff and type section
    if (file_find_type(file, name, del_type, &del_soff, &del_sect) == OK)
    {
        // Delete type
        DO_OR_FAIL(sect_types_delete(del_sect, del_soff));

        type_dtor(del_type);
        return file_head_sync((File_head *)file);
    }
    else
    {
        type_dtor(del_type);
        return FAILED;
    }
}
Status file_find_type(File *const file, const String *const name, Type *const o_type, Sectoff *const o_adr, Sect_types **o_sect_ptr)
{
    Sect_types *types = file->f_types->head;
    Sectoff soff;
    while (types != NULL)
    {
        if (sect_types_find(types, name, o_type, &soff) == OK)
        {
            *o_adr = soff;
            *o_sect_ptr = types;
            return OK;
        }
        types = types->next;
    }

    return FAILED;
}

Status file_write(struct File *const file, const Json *const json, Fileoff dad_fileoff, Fileoff type_foff, Fileoff *const wrt_foff)
{
    ENTITY_INIT(entity, json, dad_fileoff, 0, 0, type_foff);

    Sectoff wrt_soff;
    Sect_ext *extents = file->f_extent->head;
    while (extents != NULL && sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), entity, &wrt_soff) == FAILED)
    {
        extents = extents->next;
    }

    if (extents == NULL)
    {
        extents = sect_ext_new();
        DO_OR_FAIL(file_add_sect_ext(file, extents));
        DO_OR_FAIL(sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), entity, &wrt_soff));
    }
    *wrt_foff = sect_head_get_fileoff((Sect_head *)extents, wrt_soff);

    Fileoff bro_offset = 0;
    if (json->bro != NULL)
    {
        DO_OR_FAIL(file_write(file, json->bro, dad_fileoff, 0, &bro_offset));
        entity->fam_addr.bro_ptr = bro_offset;
    }

    Fileoff son_offset = 0;
    if (json->son != NULL)
    {
        DO_OR_FAIL(file_write(file, json->son, *wrt_foff, 0, &son_offset));
        entity->fam_addr.son_ptr = son_offset;
    }

    sect_ext_wrt_itm(extents, wrt_soff, entity);
    entity_dtor(entity);
    return file_head_sync((File_head *)file);
}

Status file_read(File *const file, const Fileoff fileoff, Json *const ret_json, Entity *const ret_entity)
{
    Sect_ext *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
        return FAILED;

    DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), ret_entity, ret_json));
    ret_json->foff = fileoff;

    if (ret_entity->fam_addr.bro_ptr != 0)
    {
        Json *bro_json = json_new();
        Entity entity;
        DO_OR_FAIL(file_read(file, ret_entity->fam_addr.bro_ptr, bro_json, &entity));
        json_add_bro(ret_json, bro_json);
    }

    if (ret_entity->fam_addr.son_ptr != 0)
    {
        Json *son_json = json_new();
        Entity entity;
        DO_OR_FAIL(file_read(file, ret_entity->fam_addr.son_ptr, son_json, &entity));
        json_add_son(ret_json, son_json);
    }

    return OK;
}

Status file_delete(File *const file, const Fileoff fileoff, const bool is_root)
{
    // Find root section
    Sect_ext *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
        return FAILED;

    // Delete and read del root
    Entity *del_entity = entity_new();
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
            file_head_write((File_head *)file, del_entity->fam_addr.dad_ptr + offsetof(Entity, fam_addr) + offsetof(Tplgy, son_ptr),
                            sizeof(Fileoff), (void *)&del_entity->fam_addr.bro_ptr);
        }
        // Change dad -> son to 0
        else
        {
            Fileoff temp_zero = 0;
            file_head_write((File_head *)file, del_entity->fam_addr.dad_ptr + offsetof(Entity, fam_addr) + offsetof(Tplgy, son_ptr),
                            sizeof(Fileoff), &temp_zero);
        }
    }

    entity_dtor(del_entity);
    return file_head_sync((File_head *)file);
}

Status file_update(File *const file, const Fileoff fileoff, const Json *const new_json, const Fileoff dad_ptr, const Fileoff type_ptr, bool is_root, Fileoff *cur_fileoff)
{
    Sect_ext *extents = get_sect_ext(file, fileoff);
    if (extents == NULL)
        return FAILED;

    Json *old_json = json_new();
    Entity *old_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), old_entity, old_json));

    if (json_cmp(old_json, new_json) != 0)
    {
        *cur_fileoff = fileoff;
        ENTITY_INIT(new_entity, new_json, dad_ptr, 0, 0, type_ptr);

        /*
            - Если не можем обновить, то удалим сущность из секции
            - Далее добавим сущность в первую свободную секцию
            - Обновим дерево под ней
        */
        if (sect_ext_update(extents, sect_head_get_sectoff(&extents->header, fileoff), new_json->key, json_val_ptr(new_json), json_val_size(new_json), new_entity) == FAILED)
        {
            DO_OR_FAIL(sect_ext_delete(extents, sect_head_get_sectoff(&extents->header, fileoff), NULL));

            Sectoff wrt_soff;
            Sect_ext *empty_sect_ext = file->f_extent->head;
            while (empty_sect_ext != NULL && sect_ext_write(empty_sect_ext, new_json->key, json_val_ptr(new_json), json_val_size(new_json), new_entity, &wrt_soff) == FAILED)
            {
                empty_sect_ext = empty_sect_ext->next;
            }

            if (empty_sect_ext == NULL)
            {
                empty_sect_ext = sect_ext_new();
                DO_OR_FAIL(file_add_sect_ext(file, empty_sect_ext));
                DO_OR_FAIL(sect_ext_write(extents, new_json->key, json_val_ptr(new_json), json_val_size(new_json), new_entity, &wrt_soff));
            }

            *cur_fileoff = sect_head_get_fileoff(&empty_sect_ext->header, wrt_soff);
        }
    }

    Fileoff bro_ptr = 0;
    if (!is_root && new_json->bro != NULL)
    {
        if (old_entity->fam_addr.bro_ptr)
        {
            DO_OR_FAIL(file_update(file, old_entity->fam_addr.bro_ptr, new_json->bro, *cur_fileoff, 0, false, &bro_ptr));
        }
        else
        {
            DO_OR_FAIL(file_write(file, new_json->bro, *cur_fileoff, 0, &bro_ptr));
        }
    }

    Fileoff son_ptr = 0;
    if (new_json->son != NULL)
    {
        if (old_entity->fam_addr.son_ptr)
        {
            DO_OR_FAIL(file_update(file, old_entity->fam_addr.son_ptr, new_json->son, *cur_fileoff, 0, false, &son_ptr));
        }
        else
        {
            DO_OR_FAIL(file_write(file, new_json->son, *cur_fileoff, 0, &son_ptr));
        }
    }

    if (bro_ptr)
    {
        file_head_write((File_head *)file, *cur_fileoff + offsetof(Entity, fam_addr) + offsetof(Tplgy, bro_ptr), sizeof(Fileoff), &bro_ptr);
    }
    if (son_ptr)
    {
        file_head_write((File_head *)file, *cur_fileoff + offsetof(Entity, fam_addr) + offsetof(Tplgy, son_ptr), sizeof(Fileoff), &son_ptr);
    }

    json_dtor(old_json);
    entity_dtor(old_entity);

    return file_head_sync((File_head *)file);
}

Status file_find(File *const file, Sect_ext *section, const Query *const query, List_Pair_Json_Entity *const o_obj_col)
{
    List_Pair_Json_Entity *temp = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_find(section, query, temp));

    while (temp->head != NULL)
    {
        Json *dad_json = json_new();
        Entity *dad_entity = entity_new();
        DO_OR_FAIL(file_read(file, temp->head->s->fam_addr.dad_ptr, dad_json, dad_entity));

        if (query_check_and(query, dad_json))
        {
            Pair_Json_Entity *pair = pair_Json_Entity_new();
            pair_Json_Entity_ctor(pair, dad_json, dad_entity);

            list_Pair_Json_Entity_add(o_obj_col, pair);
        }
        else
        {
            json_dtor_with_bro(dad_json);
        }

        list_Pair_Json_Entity_del_fst(temp);
    }

    list_Pair_Json_Entity_dtor(temp);

    return OK;
}

Sect_ext *get_sect_ext(const File *const file, Fileoff fileoff)
{
    Sect_ext *r_sect_ext = file->f_extent->head;
    while (r_sect_ext != NULL)
    {
        if (r_sect_ext->header.file_offset <= fileoff && r_sect_ext->header.file_offset + SECTION_SIZE > fileoff)
            return r_sect_ext;

        r_sect_ext = r_sect_ext->next;
    }
    return NULL;
}

Sect_types *get_sect_types(const File *const file, Fileoff fileoff)
{
    Sect_types *r_sect_types = file->f_types->head;
    while (r_sect_types != NULL)
    {
        if (r_sect_types->header.file_offset <= fileoff && r_sect_types->header.file_offset + SECTION_SIZE > fileoff)
            return r_sect_types;

        r_sect_types = r_sect_types->next;
    }
    return NULL;
}

Status file_add_sect_ext(File *const file, struct Sect_ext *r_extents)
{
    sect_ext_ctor(r_extents, file->header.lst_sect_ptr, file->header.filp);
    list_Sect_ext_add(file->f_extent, r_extents);

    return file_head_shift_lsp((File_head *)file, SECTION_SIZE);
}
Status file_add_sect_types(File *const file, struct Sect_types *r_types)
{
    sect_types_ctor(r_types, file->header.lst_sect_ptr, file->header.filp);
    list_Sect_types_add(file->f_types, r_types);

    return file_head_shift_lsp((File_head *)file, SECTION_SIZE);
}