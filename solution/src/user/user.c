#include "user/user.h"

#include "file/file_p.h"

#include "section/header.h"
#include "section/header_p.h"

#include "section/types.h"
#include "section/types_p.h"

Status user_add_type(struct File *const file, const Type *const type)
{
    Fileoff wrt_adr;
    return file_add_type(file, type, &wrt_adr);
}
Status user_delete_type(struct File *const file, const String *const name)
{
    return file_delete_type(file, name);
}
Status user_read_type(struct File *const file, const String *const name, Type *const o_type)
{
    Sectoff t_soff;
    struct Sect_types *sect;
    return file_find_type(file, name, o_type, &t_soff, &sect);
}

Status user_write(struct File *const file, const Json *const json, const String *const type_name)
{
    Type *t = type_new();
    Sectoff soff;
    Sect_types *types;
    DO_OR_FAIL(file_find_type(file, type_name, t, &soff, &types));

    if (json_is_apply_type(json, t) == OK)
    {
        Fileoff wrt_foff;
        return file_write(file, json, 0, sect_head_get_fileoff((Sect_head *)types, soff), &wrt_foff);
    }
    return FAILED;
}

struct Iter *user_read(struct File *const file, Query *const query)
{
    struct Iter *iter = iter_new();
    iter_ctor(iter, file, query);

    return iter;
}

Status user_delete(struct File *const file, Query *const query)
{
    struct Iter *iter = iter_new();
    iter_ctor(iter, file, query);

    while (iter_is_avail(iter))
    {
        DO_OR_FAIL(file_delete(file, iter_get_json(iter)->foff, true));

        iter_next(iter);
    }

    return OK;
}

Status user_update(struct File *const file, Query *const query, const Json *const new_json)
{
    struct Iter *iter = iter_new();
    iter_ctor(iter, file, query);

    Fileoff upd_fileoff;
    while (iter_is_avail(iter))
    {
        DO_OR_FAIL(file_delete(file, iter_get_json(iter)->foff, true));
        DO_OR_FAIL(file_write(file, new_json, iter_get_entity(iter)->fam_addr.dad_ptr, iter_get_entity(iter)->type_ptr, &upd_fileoff) == FAILED);

        iter_next(iter);
    }

    iter_dtor(iter);

    return OK;
}