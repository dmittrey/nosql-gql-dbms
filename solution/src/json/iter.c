#include <string.h>

#include "memory/json/iter.h"

#include "memory/section/extents.h"

iter_t *iter_new()
{
    return memset(my_malloc(iter_t), 0, sizeof(iter_t));
}

void iter_ctor(iter_t *const iter, const file_t *const file, const query_t *const query)
{
    iter->file = file;
    iter->query = query;
    iter->cur_sect = file->f_extent;
    iter->cur_json_col = json_col_new();
}
void iter_dtor(iter_t *iter)
{
    query_dtor(iter->query);
    json_col_dtor(iter->cur_json_col);
    free(iter);
}

status_t iter_next(iter_t *const iter)
{
    if (iter->cur_json_col->count == 0)
    {
        sect_ext_t *prev = iter->file->f_extent;

        if (iter->is_loaded_cur_sect == true)
        {
            iter->cur_sect = iter->cur_sect->next;
            iter->file->f_extent = iter->cur_sect;
        }

        status_t find_st = file_find(iter->file, iter->query, iter->cur_json_col);
        iter->file->f_extent = prev;

        iter->is_loaded_cur_sect = find_st;
        DO_OR_FAIL(find_st);
    }

    if (iter->cur_json_col->f_json != NULL)
    {
        iter->cur_json_col->f_json = iter->cur_json_col->f_json->next;
    }
    else
    {
        return FAILED;
    }

    return OK;
}
json_t *iter_get(iter_t *const iter)
{
    return iter->cur_json_col->f_json;
}
