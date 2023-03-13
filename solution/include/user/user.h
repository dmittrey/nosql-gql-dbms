#pragma once

#include "memory/file/file.h"

#include "memory/json/iter.h"

#include "memory/type/type.h"

status_t user_add_type(file_t *const file, const type_t *const type);
status_t user_delete_type(file_t *const file, const string_t *const name);
status_t user_find_type(file_t *const file, const string_t *const name, type_t *const o_type);

status_t user_write(file_t *const file, const json_t *const json);
struct iter_t *user_read(file_t *const file, query_t *const query);
status_t user_update(file_t *const file, query_t *const query, const json_t *const new_json);
status_t user_delete(file_t *const file, query_t *const query);