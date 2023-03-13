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
    sect_type_t* del_types = NULL;
    return file_find_type(file, name, o_type, del_types);
}

status_t user_write(file_t *const file, const json_t *const json)
{
    fileoff_t wrt_adr;
    return file_write(file, json, 0, &wrt_adr);
}
iter_t *user_read(file_t *const file, query_t *const query)
{
    iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    return iter;
}
status_t user_update(file_t *const file, query_t *const query, const json_t *const new_json)
{
    iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    // Iterate over json
    //  And delete in fileoff(json field)

    return OK;
}
status_t user_delete(file_t *const file, query_t *const query)
{
    iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    // Iterate over json
    //  And delete in fileoff(json field)

    return OK;
}