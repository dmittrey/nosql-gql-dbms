#include "user/user.h"

status_t user_write(file_t *const file, const json_t *const json)
{
    fileoff_t wrt_addr;
    return file_write(file, json, 0, &wrt_addr);
}

iter_t *user_read(file_t *const file, const query_t *const query)
{
    
}

status_t user_update(file_t *const file, const query_t *const query, const json_t *const new_json)
{

}

status_t user_delete(file_t *const file, const query_t *const query)
{

}