#include "utils/table.h"

#include "section/header.h"
#include "section/header_p.h"

Sect_head *sect_head_new()
{
    return my_calloc(Sect_head);
}

Status sect_head_ctor(Sect_head *const header, const Fileoff offset, FILE *const filp)
{
    header->free_space = SECTION_SIZE - sizeof(Sect_head_entity);
    header->next_ptr = 0; // If we have 0 then we don't have next section
    header->lst_itm_ptr = sizeof(Sect_head_entity);
    header->fst_rec_ptr = SECTION_SIZE;
    header->file_offset = offset;
    header->filp = filp;

    return sect_head_sync(header);
}

Status sect_head_dtor(Sect_head *header)
{
    header->free_space = 0;
    header->next_ptr = 0;
    header->lst_itm_ptr = 0;
    header->fst_rec_ptr = 0;

    DO_OR_FAIL(sect_head_sync(header));

    free(header);

    return OK;
}

Status sect_head_sync(Sect_head *const header)
{
    SAVE_FILP(header->filp,
              RA_FWRITE_OR_FAIL(header, sizeof(Sect_head_entity), header->file_offset, header->filp));

    return OK;
}

Status sect_head_shift_lip(Sect_head *const header, const size_t shift)
{
    header->free_space -= shift;
    header->lst_itm_ptr += shift;

    return sect_head_sync(header);
}

Status sect_head_shift_frp(Sect_head *const header, const size_t shift)
{
    header->free_space += shift;
    header->fst_rec_ptr += shift;

    return sect_head_sync(header);
}

Status sect_head_cmprs_lip(Sect_head *const header, const size_t itm_size)
{
    void *rd_data = my_malloc_array(char, itm_size);
    void *zr_data = memset(my_malloc_array(char, itm_size), 0, itm_size);

    sect_head_read(header, header->lst_itm_ptr - itm_size, itm_size, rd_data);

    while (header->lst_itm_ptr != sizeof(Sect_head_entity) && memcmp(rd_data, zr_data, itm_size) == 0)
    {
        header->lst_itm_ptr -= itm_size;
        header->free_space += itm_size;

        sect_head_read(header, header->lst_itm_ptr - itm_size, itm_size, rd_data);
    }

    free(rd_data);
    free(zr_data);

    return sect_head_sync(header);
}

Status sect_head_cmprs_frp(Sect_head *const header)
{
    char chr;
    while (true)
    {
        sect_head_read(header, header->fst_rec_ptr, sizeof(char), &chr);

        if (header->fst_rec_ptr == SECTION_SIZE || chr != 0)
            break;

        header->fst_rec_ptr += 1;
        header->free_space += 1;
    }

    return sect_head_sync(header);
}

Status sect_head_read(Sect_head *const header, const Sectoff soff, const size_t sz, void *const o_data)
{
    if (sz != 0)
    {
        SAVE_FILP(header->filp, {
            RA_FREAD_OR_FAIL(o_data, sz, sect_head_get_fileoff(header, soff), header->filp);
        });
    }

    return OK;
}

Status sect_head_write(Sect_head *const header, const Sectoff soff, const size_t sz, const void *const data)
{
    if (sz != 0)
    {
        SAVE_FILP(header->filp, {
            RA_FWRITE_OR_FAIL(data, sz, sect_head_get_fileoff(header, soff), header->filp);
        });
    }

    return OK;
}

Fileoff sect_head_get_fileoff(const Sect_head *const header, const Sectoff offset)
{
    return header->file_offset + offset;
}

Sectoff sect_head_get_sectoff(const Sect_head *const header, const Fileoff offset)
{
    return offset - header->file_offset;
}

int sect_head_cmp(Sect_head *this, Sect_head *other)
{
    int cmp_flag;

    cmp_flag = memcmp((void *)this->file_offset, (void *)other->file_offset, sizeof(Fileoff));
    if (cmp_flag)
        return cmp_flag;
    cmp_flag = memcmp((void *)this->free_space, (void *)other->free_space, sizeof(size_t));
    if (cmp_flag)
        return cmp_flag;
    cmp_flag = memcmp((void *)this->fst_rec_ptr, (void *)other->fst_rec_ptr, sizeof(Sectoff));
    if (cmp_flag)
        return cmp_flag;
    cmp_flag = memcmp((void *)this->lst_itm_ptr, (void *)other->lst_itm_ptr, sizeof(Sectoff));
    if (cmp_flag)
        return cmp_flag;

    return 0;
}