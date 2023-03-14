#include "user/user.h"

status_t user_add_type(file_t *const file, const type_t *const type)
{
    return file_add_type(file, type);
}
status_t user_delete_type(file_t *const file, const string_t *const name)
{
    return file_delete_type(file, name);
}
status_t user_find_type(file_t *const file, const string_t *const name, type_t *const o_type)
{
    sect_type_t *del_types = NULL;
    return file_find_type(file, name, o_type, &del_types);
}

status_t user_write(file_t *const file, const json_t *const json, const string_t *const t_name)
{
    type_t *t = type_new();
    sect_type_t *t_sect;
    file_find_type(file, t_name, t, &t_sect);

    bool is_apply = json_is_apply_type(json, t);

    if (is_apply)
    {
        fileoff_t wrt_adr;
        return file_write(file, json, 0, &wrt_adr);
    }
    else
    {
        return FAILED;
    }
}
struct iter_t *user_read(file_t *const file, query_t *const query)
{
    struct iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    return iter;
}
status_t user_update(file_t *const file, query_t *const query, const json_t *const new_json)
{
    struct iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    fileoff_t upd_fileoff;
    while (iter_is_avail(iter))
    {
        json_t *json = iter_get(iter);
        if (json_is_apply_type(new_json, json->obj_type))
        {
            file_update(file, iter_get(iter)->foff, new_json, 0, false, &upd_fileoff);
        }
        iter_next(iter);
    }
    iter_dtor(iter);

    return OK;
}
status_t user_delete(file_t *const file, query_t *const query)
{
    struct iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    fileoff_t upd_fileoff;
    while (iter_is_avail(iter))
    {
        file_delete(file, iter_get(iter)->foff, true);
        iter_next(iter);
    }
    iter_dtor(iter);

    return OK;
}