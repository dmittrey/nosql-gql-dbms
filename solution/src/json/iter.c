#include "json/iter.h"

#include "section/extents_p.h"

#include "file/file.h"
#include "file/file_p.h"


typedef struct Iter
{
    File *file;
    Query *query;
    List_Pair_Json_Entity *cur_col;
    Sect_ext *cur_sect;
} Iter;

Iter *iter_new()
{
    return my_calloc(Iter);
}

// void iter_ctor(Iter *const iter, File *const file, Query *const query)
// {
//     iter->file = file;
//     iter->query = query;

//     iter->cur_sect = file->f_extent->head;
//     iter->cur_col = list_Pair_Json_Entity_new();

//     if (iter->cur_sect != NULL)
//     {
//         file_find(file, iter->cur_sect, query, iter->cur_col);
//     }
// }
// void iter_dtor(Iter *iter)
// {
//     list_Pair_Json_Entity_dtor(iter->cur_col);
//     free(iter);
// }

// void iter_next(Iter *const iter)
// {
//     if (iter->cur_col->count != 0)
//     {
//         list_Pair_Json_Entity_del_fst(iter->cur_col);
//     }

//     if (iter->cur_col->count == 0)
//     {
//         while (iter->cur_sect != NULL && iter->cur_col->count == 0)
//         {
//             iter->cur_sect = iter->cur_sect->next;
//             file_find(iter->file, iter->cur_sect, iter->query, iter->cur_col);
//         }
//     }
// }

// bool iter_is_avail(Iter *const iter)
// {
//     return iter->cur_col->count > 0;
// }

// Json *iter_get_json(Iter *const iter)
// {
//     return iter->cur_col->head->f;
// }

// Entity *iter_get_entity(Iter *const iter)
// {
//     return iter->cur_col->head->s;
// }
