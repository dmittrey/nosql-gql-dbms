#include <string.h>

#include "memory/json/iter.h"

#include "memory/section/extents.h"

typedef struct iter_t
{
    file_t *file;
    query_t *query;
    list_json_t *cur_json_col;
    sect_ext_t *cur_sect;
} iter_t;

iter_t *iter_new()
{
    return memset(my_malloc(iter_t), 0, sizeof(iter_t));
}

void iter_ctor(iter_t *const iter, file_t *const file, query_t *const query)
{
    iter->file = file;
    iter->query = query;

    iter->cur_sect = file->f_extent;
    iter->cur_json_col = list_json_t_new();

    if (iter->cur_sect != NULL)
    {
        file_find(file, iter->cur_sect, query, iter->cur_json_col);
    }
}
void iter_dtor(iter_t *iter)
{
    query_dtor(iter->query);
    list_json_t_dtor(iter->cur_json_col);
    free(iter);
}

void iter_next(iter_t *const iter)
{
    if (iter->cur_json_col->head->next != NULL)
    {
        list_json_t_del_fst(iter->cur_json_col);
    }
    else
    {
        while (iter->cur_sect != NULL && iter->cur_json_col->count == 0)
        {
            iter->cur_sect = iter->cur_sect->next;
            file_find(iter->file, iter->cur_sect, iter->query, iter->cur_json_col);
        }
    }
}

bool iter_is_avail(iter_t *const iter)
{
    return iter->cur_json_col->count > 0;
}

/*
Null or json
*/
json_t *iter_get(iter_t *const iter)
{
    return iter->cur_json_col->head;
}
