#include "file/file.h"

file_t *file_new();

void file_ctor(file_t *const file, FILE *const filp);
PerformStatus file_dtor(file_t * file);

PerformStatus file_write(file_t *const file, const json_value_t *const json);
PerformStatus file_read(file_t *const file, const char *path, json_value_t *const ret_json);
PerformStatus file_update(file_t *const file, const char *path, const json_value_t *const new_json);
PerformStatus file_delete(file_t *const file, const char *path);