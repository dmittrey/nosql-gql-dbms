#pragma once

#include "memory/file/file.h"

#include "memory/json/iter.h"

status_t user_write(file_t *const file, const json_t *const json);
iter_t *user_read(file_t *const file, const query_t *const query);
status_t user_update(file_t *const file, const query_t *const query, const json_t *const new_json);
status_t user_delete(file_t *const file, const query_t *const query);