#include "user/user.h"

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
    return file_find_type(file, name, o_type, &t_soff, sect);
}