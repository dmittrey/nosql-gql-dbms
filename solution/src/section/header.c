#include "memory/section/header.h"
#include "memory/section/header_p.h"

#include "physical/section/header.h"

sect_head_t *sect_head_new()
{
    return my_calloc(sect_head_t);
}

status_t sect_head_ctor(sect_head_t *const header, const fileoff_t offset, FILE *const filp)
{
    header->free_space = SECTION_SIZE - sizeof(sect_head_entity_t);
    header->next_ptr = 0; // If we have 0 then we don't have next section
    header->lst_itm_ptr = sizeof(sect_head_entity_t);
    header->fst_rec_ptr = SECTION_SIZE;
    header->sect_off = offset;
    header->filp = filp;

    return sect_head_sync(header);
}

status_t sect_head_dtor(sect_head_t *header)
{
    header->free_space = 0;
    header->next_ptr = 0;
    header->lst_itm_ptr = 0;
    header->fst_rec_ptr = 0;
    
    DO_OR_FAIL(sect_head_sync(header));

    free(header);

    return OK;
}

status_t sect_head_sync(sect_head_t *const header)
{
    SAVE_FILP(header->filp,
              RA_FWRITE_OR_FAIL(header, sizeof(sect_head_entity_t), header->sect_off, header->filp));

    return OK;
}

status_t sect_head_shift_lip(sect_head_t *const header, const size_t shift)
{
    header->free_space -= shift;
    header->lst_itm_ptr += shift;

    return sect_head_sync(header);
}

status_t sect_head_shift_frp(sect_head_t *const header, const size_t shift)
{
    header->free_space += shift;
    header->fst_rec_ptr += shift;

    return sect_head_sync(header);
}

status_t sect_head_cmprs_lip(sect_head_t *header, size_t itm_size)
{
    void *rd_data = my_malloc_array(char, itm_size);
    void *zr_data = memset(my_malloc_array(char, itm_size), 0, itm_size);

    sect_head_read(header, header->lst_itm_ptr - itm_size, itm_size, rd_data);

    while (header->lst_itm_ptr != sizeof(sect_head_entity_t) && memcmp(rd_data, zr_data, itm_size) == 0)
    {
        header->lst_itm_ptr -= itm_size;
        header->free_space += itm_size;

        sect_head_read(header, header->lst_itm_ptr - itm_size, itm_size, rd_data);
    }

    free(rd_data);
    free(zr_data);

    return sect_head_sync(header);
}

status_t sect_head_cmprs_frp(sect_head_t *header)
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

status_t sect_head_read(sect_head_t *header, sectoff_t sectoff, size_t sz, void *o_data)
{
    SAVE_FILP(header->filp, {
        RA_FREAD_OR_FAIL(o_data, sz, sect_head_get_fileoff(header, sectoff), header->filp);
    });

    return OK;
}

status_t sect_head_write(sect_head_t *header, sectoff_t sectoff, size_t sz, void *data)
{
    if (sz != 0)
    {
        SAVE_FILP(header->filp, {
            RA_FWRITE_OR_FAIL(data, sz, sect_head_get_fileoff(header, sectoff), header->filp);
        });
    }

    return OK;
}

fileoff_t sect_head_get_fileoff(const sect_head_t *const header, const sectoff_t offset)
{
    return header->sect_off + offset;
}

sectoff_t sect_head_get_sectoff(const sect_head_t *const header, const fileoff_t offset)
{
    return offset - header->sect_off;
}