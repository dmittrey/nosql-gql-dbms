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

// /*
// cur -> bro -> son
// */
// status_t file_write(file_t *const file, const json_t *const json, fileoff_t dad_fileoff, fileoff_t type_foff, fileoff_t *const write_addr)
// {
//     ENTITY_INIT(entity, json, dad_fileoff, 0, 0);
//     entity->type_ptr = type_foff;

//     sect_ext_t *extents = find_sect_ext(file, entity_ph_size(entity));
//     if (extents == NULL)
//     {
//         extents = sect_ext_new();
//         DO_OR_FAIL(file_add_sect_ext(file, extents));
//     }

//     sect_ext_write(extents, json, entity, write_addr);

//     *write_addr += extents->header.sect_off;

//     fileoff_t bro_offset = 0;
//     if (json->bro != NULL)
//     {
//         DO_OR_FAIL(file_write(file, json->bro, dad_fileoff, type_foff, &bro_offset));
//         entity->fam_addr.bro_ptr = bro_offset;
//     }

//     fileoff_t son_offset = 0;
//     if (json->son != NULL)
//     {
//         DO_OR_FAIL(file_write(file, json->son, *write_addr, type_foff, &son_offset));
//         entity->fam_addr.son_ptr = son_offset;
//     }

//     if (bro_offset)
//     {
//         RA_FWRITE_OR_FAIL(&bro_offset, sizeof(bro_offset), *write_addr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, bro_ptr), file->filp);
//     }
//     if (son_offset)
//     {
//         RA_FWRITE_OR_FAIL(&son_offset, sizeof(son_offset), *write_addr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
//     }

//     entity_dtor(entity);

//     return file_head_sync((File_head *)file);
// }
// /*
// 1) Прочитать секцию с нодой
// 2) Прочитать ноду по секционному смещению
// 3) Если есть брат, запуск от брата
// 4) Прокинуть ссылку на брата
// 5) Если есть сын, запуск от сына
// 6) Прокинуть ссылку на сына
// */
// status_t file_read(file_t *const file, const fileoff_t fileoff, json_t *const ret_json)
// {
//     sect_ext_t *extents = get_sect_ext(file, fileoff);
//     if (extents == NULL)
//         return FAILED;

//     entity_t *ret_entity = entity_new();
//     DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), ret_entity, ret_json));
//     ret_json->entity = ret_entity;

//     if (ret_entity->fam_addr.bro_ptr != 0)
//     {
//         json_t *bro_json = json_new();
//         DO_OR_FAIL(file_read(file, ret_entity->fam_addr.bro_ptr, bro_json));
//         json_add_bro(ret_json, bro_json);
//     }

//     if (ret_entity->fam_addr.son_ptr != 0)
//     {
//         json_t *son_json = json_new();
//         DO_OR_FAIL(file_read(file, ret_entity->fam_addr.son_ptr, son_json));
//         json_add_son(ret_json, son_json);
//     }

//     return file_head_sync((File_head *)file);
// }

// /*
// 1) Считали старую ноду
// 2) Сравнили ноду, если надо, то обновили
// 3) Обновили поддерево
// */
// status_t file_update(file_t *const file, const fileoff_t fileoff, const json_t *const new_json, const fileoff_t dad_ptr, bool is_bro_upd, fileoff_t *cur_fileoff)
// {
//     sect_ext_t *extents = get_sect_ext(file, fileoff);
//     if (extents == NULL)
//     {
//         return FAILED;
//     }

//     json_t *old_json = json_new();
//     entity_t *old_entity = entity_new();
//     DO_OR_FAIL(sect_ext_read(extents, sect_head_get_sectoff(&extents->header, fileoff), old_entity, old_json));

//     *cur_fileoff = fileoff;
//     ENTITY_INIT(new_entity, new_json, dad_ptr, 0, 0);

//     if (json_cmp(old_json, new_json) != 0)
//     {
//         /*
//             - Если не можем обновить, то удалим сущность из секции
//             - Далее добавим сущность в первую свободную секцию
//             - Обновим дерево под ней
//         */
//         if (sect_ext_update(extents, sect_head_get_sectoff(&extents->header, fileoff), new_json, new_entity) == FAILED)
//         {
//             DO_OR_FAIL(sect_ext_delete(extents, sect_head_get_sectoff(&extents->header, fileoff), NULL));

//             sect_ext_t *empty_sect_ext = find_sect_ext(file, entity_ph_size(new_entity));
//             if (empty_sect_ext == NULL)
//             {
//                 empty_sect_ext = sect_ext_new();
//                 DO_OR_FAIL(file_add_sect_ext(file, empty_sect_ext));
//             }

//             sectoff_t save_addr;
//             DO_OR_FAIL(sect_ext_write(empty_sect_ext, new_json, new_entity, &save_addr));

//             *cur_fileoff = sect_head_get_fileoff(&empty_sect_ext->header, save_addr);
//         }
//     }

//     fileoff_t bro_ptr = 0;
//     if (is_bro_upd && new_json->bro != NULL)
//     {
//         DO_OR_FAIL(file_update(file, old_entity->fam_addr.bro_ptr, new_json->bro, *cur_fileoff, true, &bro_ptr));
//     }

//     fileoff_t son_ptr = 0;
//     if (new_json->son != NULL)
//     {
//         DO_OR_FAIL(file_update(file, old_entity->fam_addr.son_ptr, new_json->son, *cur_fileoff, true, &son_ptr));
//     }

//     if (bro_ptr)
//     {
//         RA_FWRITE_OR_FAIL(&bro_ptr, sizeof(bro_ptr), *cur_fileoff + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, bro_ptr), file->filp);
//     }
//     if (son_ptr)
//     {
//         RA_FWRITE_OR_FAIL(&son_ptr, sizeof(son_ptr), *cur_fileoff + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
//     }

//     json_dtor(old_json);
//     entity_dtor(old_entity);
//     entity_dtor(new_entity);

//     return file_head_sync((File_head *)file);
// }

// /*
// 1) Нашли секцию с удаляемой нодой, если не нашли то FAILED
// 2) Удалили текущую ноду и вернули ее сущность
// 3) Удалили дерево под ней
// 4) Поменяли ссылку с отца на сына
// */
// status_t file_delete(file_t *const file, const fileoff_t fileoff, bool is_root)
// {
//     // Find root section
//     sect_ext_t *extents = get_sect_ext(file, fileoff);
//     if (extents == NULL)
//     {
//         return FAILED;
//     }

//     // Delete and read del root
//     entity_t *const del_entity = entity_new();
//     DO_OR_FAIL(sect_ext_delete(extents, sect_head_get_sectoff(&extents->header, fileoff), del_entity));

//     if (!is_root && del_entity->fam_addr.bro_ptr != 0)
//     {
//         DO_OR_FAIL(file_delete(file, del_entity->fam_addr.bro_ptr, false));
//     }

//     // Delete recursively under tree
//     if (del_entity->fam_addr.son_ptr != 0)
//     {
//         DO_OR_FAIL(file_delete(file, del_entity->fam_addr.son_ptr, false));
//     }

//     // If dad exist change dad -> son ptr
//     if (is_root && del_entity->fam_addr.dad_ptr != 0)
//     {
//         // Change dad -> son to bro_ptr
//         if (del_entity->fam_addr.bro_ptr != 0)
//         {
//             RA_FWRITE_OR_FAIL(&del_entity->fam_addr.bro_ptr, sizeof(fileoff_t), del_entity->fam_addr.dad_ptr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
//         }
//         // Change dad -> son to 0
//         else
//         {
//             fileoff_t temp_zero = 0;
//             RA_FWRITE_OR_FAIL(&temp_zero, sizeof(fileoff_t), del_entity->fam_addr.dad_ptr + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
//         }
//     }

//     entity_dtor(del_entity);

//     return file_head_sync((File_head *)file);
// }

// /*
// 1) Получили массив json'ов которые удовлетворяют хотя бы одному условию
// 2) Итерируемся по каждому
//     - Прочитали по ссылке на родителя
//     - Прогнали по всем условиям
//     - Если соответствует то сохранили в o_obj_col
// */
// status_t file_find(file_t *const file, sect_ext_t *section, const query_t *const query, list_json_t *const o_obj_col)
// {
//     if (section == NULL)
//         return OK;

//     list_json_t *temp = list_json_t_new();
//     DO_OR_FAIL(sect_ext_find(section, query, temp));

//     while (temp->head != NULL)
//     {
//         json_t *dad_json = json_new();
//         DO_OR_FAIL(file_read(file, temp->head->entity->fam_addr.dad_ptr, dad_json));

//         if (query_check_and(query, dad_json))
//         {
//             if (!list_json_t_add_lk_set(o_obj_col, dad_json))
//             {
//                 json_dtor_with_bro(dad_json);
//             }
//         }
//         else
//         {
//             json_dtor_with_bro(dad_json);
//         }

//         list_json_t_del_fst(temp);
//     }

//     list_json_t_dtor(temp);

//     return OK;
// }

// /*
// Add empty extents section to end of file
// */
// status_t file_add_sect_ext(file_t *const file, sect_ext_t *new_extents)
// {
//     DO_OR_FAIL(sect_ext_ctor(new_extents, file->header.lst_sect_ptr, file->filp));
//     file->header.lst_sect_ptr += SECTION_SIZE;
//     if (file->f_extent == NULL)
//     {
//         file->f_extent = new_extents;
//         return OK;
//     }

//     sect_ext_t *cur = file->f_extent;
//     while (cur->next != NULL)
//     {
//         cur = cur->next;
//     }
//     cur->next = new_extents;
//     cur->header.next_ptr = new_extents->header.sect_off;
//     DO_OR_FAIL(sect_ext_sync(cur));

//     return file_head_sync((File_head *)file);
// }

// /*
// Add empty types section to end of file
// */
// status_t file_add_sect_types(file_t *const file, sect_type_t *new_types)
// {
//     DO_OR_FAIL(sect_type_ctor(new_types, file->header.lst_sect_ptr, file->filp));
//     file->header.lst_sect_ptr += SECTION_SIZE;
//     if (file->f_types == NULL)
//     {
//         file->f_types = new_types;
//         return OK;
//     }

//     sect_type_t *cur = file->f_types;
//     while (cur->next != NULL)
//     {
//         cur = cur->next;
//     }
//     cur->next = new_types;
//     cur->header.next_ptr = new_types->header.sect_off;
//     DO_OR_FAIL(sect_types_sync(cur));

//     return file_head_sync((File_head *)file);
// }

// static sect_ext_t *get_sect_ext(const file_t *const file, fileoff_t fileoff)
// {
//     sect_ext_t *r_sect_ext = file->f_extent;

//     while (r_sect_ext != NULL)
//     {
//         if (r_sect_ext->header.sect_off <= fileoff && r_sect_ext->header.sect_off + SECTION_SIZE > fileoff)
//         {
//             break;
//         }

//         r_sect_ext = r_sect_ext->next;
//     }

//     return r_sect_ext;
// }

// static sect_ext_t *find_sect_ext(const file_t *const file, const size_t entity_size)
// {
//     sect_ext_t *cur_ext = file->f_extent;
//     while (cur_ext != NULL && cur_ext->header.free_space < entity_size)
//     {
//         cur_ext = cur_ext->next;
//     }

//     return cur_ext;
// }

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