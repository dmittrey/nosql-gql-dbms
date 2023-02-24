#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"

#include "physical/section/header.h"

sect_head_t *sect_head_new()
{
    return memset(my_malloc(sect_head_t), 0, sizeof(sect_head_t));
}

status_t sect_head_ctor(sect_head_t *const header, const fileoff_t offset, FILE *const filp)
{
    header->free_space = SECTION_SIZE - sizeof(sect_head_entity_t);
    header->next_ptr = 0; // If we have 0 then we don't have next section
    header->lst_itm_ptr = offset + sizeof(sect_head_entity_t);
    header->fst_rec_ptr = offset + SECTION_SIZE;
    header->sect_off = offset;
    header->filp = filp;

    return sect_head_sync(header);
}

void sect_head_dtor(sect_head_t *header)
{
    free(header);
}

status_t sect_head_shift_lst_itm_ptr(sect_head_t *const header, const int64_t shift)
{
    header->free_space -= shift;
    header->lst_itm_ptr += shift;

    return sect_head_sync(header);
}

status_t sect_head_shift_fst_rec_ptr(sect_head_t *const header, const int64_t shift)
{
    header->free_space += shift;
    header->fst_rec_ptr += shift;

    return sect_head_sync(header);
}

status_t sect_head_sync(sect_head_t *const header)
{
    SAVE_FILP(header->filp,
              RA_FWRITE_OR_FAIL(header, sizeof(sect_head_entity_t), header->sect_off, header->filp));

    return OK;
}