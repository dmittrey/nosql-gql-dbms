#include "file/header.h"
#include "file/header_p.h"

File_head *file_head_new()
{
    return my_calloc(File_head);
}

Status file_head_ctor(File_head *const header, FILE *const filp)
{
    header->filp = filp;
    header->lst_sect_ptr = sizeof(File_head_entity);
    return file_head_sync(header);
}
Status file_head_dtor(File_head *header)
{
    header->lst_sect_ptr = 0;
    DO_OR_FAIL(file_head_sync(header));

    fclose(header->filp);
    free(header);
    return OK;
}

Status file_head_read(struct File_head *const header, const Fileoff foff, const size_t sz, void *const o_data)
{
    SAVE_FILP(header->filp, {
        RA_FREAD_OR_FAIL(o_data, sz, foff, header->filp);
    });

    return OK;
}
Status file_head_write(struct File_head *const header, const Fileoff foff, const size_t sz, const void *const data)
{
    if (sz != 0)
    {
        SAVE_FILP(header->filp, {
            RA_FWRITE_OR_FAIL(data, sz, foff, header->filp);
        });
    }

    return OK;
}

Status file_head_sync(File_head *const header)
{
    SAVE_FILP(header->filp,
              RA_FWRITE_OR_FAIL(header, sizeof(File_head_entity), 0, header->filp));

    return OK;
}

Status file_head_shift_lsp(File_head *const header, const size_t shift)
{
    header->lst_sect_ptr += shift;
    return file_head_sync(header);
}