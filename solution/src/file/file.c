#include "memory/file/file.h"

static sect_ext_t *get_sect_ext(const file_t *const file, fileoff_t fileoff);
static sect_ext_t *find_sect_ext(const file_t *const file, size_t entity_size);

file_t *file_new()
{
    return my_malloc(file_t);
}

void file_ctor(file_t *const file, FILE *const filp)
{
    file->lst_sect_ptr = sizeof(file->lst_sect_ptr);
    file->f_extent = NULL;
    file->filp = filp;
}
void file_dtor(file_t *file)
{
    free(file);
}

/*
1) Запустили запись от брата
2) Запустили запись от сына
3) Нашли секцию в которой есть пространство для записи
    Если не нашли то добавили в конец файла новую секцию
5) Записали адрес текущей сущности в записанного брата
6) Записали адрес текущей сущности в записанного сына
*/
status_t file_write(file_t *const file, const json_t *const json, fileoff_t dad_fileoff, const fileoff_t *write_addr)
{
    ENTITY_INIT(entity, json, dad_fileoff, 0, 0);

    fileoff_t bro_offset = 0;
    if (json->bro != NULL)
    {
        DO_OR_FAIL(file_write(file, json, 0, &bro_offset));
        entity->fam_addr.bro_ptr = bro_offset;
    }

    fileoff_t son_offset = 0;
    if (json->son != NULL)
    {
        DO_OR_FAIL(file_write(file, json, 0, &son_offset));
        entity->fam_addr.son_ptr = son_offset;
    }

    sect_ext_t *extents = find_sect_ext(file, entity_ph_size(entity));
    if (extents == NULL)
    {
        extents = sect_ext_new();
        DO_OR_FAIL(file_add_sect_ext(file, extents));
    }

    fileoff_t cur_offset;
    sect_ext_write(extents, json, entity, &cur_offset);

    if (bro_offset)
    {
        RA_FWRITE_OR_FAIL(&cur_offset, sizeof(cur_offset), bro_offset + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, bro_ptr), file->filp);
    }

    if (son_offset)
    {
        RA_FWRITE_OR_FAIL(&cur_offset, sizeof(cur_offset), son_offset + offsetof(entity_t, fam_addr) + offsetof(tplgy_addr, son_ptr), file->filp);
    }

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
    if (extents == NULL) return FAILED; // Заменить на дополнение секциями до fileoff 
                                        // и выборка последней секции

    sectoff_t sectoff = fileoff - extents->header.sect_off;

    entity_t *entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, sectoff, entity, ret_json));

    if (entity->fam_addr.bro_ptr != 0)
    {
        json_t *bro_json = json_new();
        DO_OR_FAIL(file_read(file, entity->fam_addr.bro_ptr, bro_json));
        ret_json->bro = bro_json;
    }

    if (entity->fam_addr.son_ptr != 0)
    {
        json_t *son_json = json_new();
        DO_OR_FAIL(file_read(file, entity->fam_addr.son_ptr, son_json));
        ret_json->son = son_json;
    }

    return OK;
}
status_t file_update(file_t *const file, const fileoff_t fileoff, const json_t *const new_json);
status_t file_delete(file_t *const file, const fileoff_t fileoff);

/*
Add empty extents section to end of file
*/
status_t file_add_sect_ext(file_t *const file, sect_ext_t *r_extents)
{
    DO_OR_FAIL(sect_ext_ctor(r_extents, file->lst_sect_ptr, file->filp));
    ADD_SECT_EXT_TO_END(file->f_extent, r_extents);
    return OK;
}
status_t file_read_sect_ext(file_t *const file, const fileoff_t fileoff, sect_ext_t *const r_sect_ext)
{
    RA_FREAD_OR_FAIL(r_sect_ext, sizeof(r_sect_ext), fileoff, file->filp);
    return OK;
}

static sect_ext_t *get_sect_ext(const file_t *const file, fileoff_t fileoff)
{
    for (size_t i = 0; i < fileoff; i += SECTION_SIZE)
    {
        // Если мы не ушли за пределы файла и fileoff в секции
        if (i < file->lst_sect_ptr && i < fileoff && (i + SECTION_SIZE) > fileoff)
        {
            sect_ext_t *extents = sect_ext_new();
            RA_FREAD_OR_FAIL(extents, sizeof(sect_ext_t), (void *)i, file->filp);
            return extents;
        }
    }

    return NULL;
}
static sect_ext_t *find_sect_ext(const file_t *const file, size_t entity_size)
{
    sect_ext_t *cur_ext = file->f_extent;
    while (cur_ext != NULL && cur_ext->header.free_space < entity_size)
    {
        cur_ext = cur_ext->next;
    }

    return cur_ext;
}