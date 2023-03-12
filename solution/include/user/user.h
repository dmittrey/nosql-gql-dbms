#pragma once

#include "memory/file/file.h"

#include "memory/json/iter.h"

#include "memory/type/type.h"

status_t user_json_type_add(file_t *const file, const type_t* const type);
status_t user_json_type_delete(file_t *const file, const string_t *const name);
status_t user_json_type_find(file_t *const file, const string_t *const name, type_t* const o_type);

status_t user_write(file_t *const file, const json_t *const json);
iter_t *user_read(file_t *const file, const query_t *const query);
status_t user_update(file_t *const file, const query_t *const query, const json_t *const new_json);
status_t user_delete(file_t *const file, const query_t *const query);