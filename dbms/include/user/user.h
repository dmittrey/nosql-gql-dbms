#pragma once

#include "utils/utils.h"

#include "json/iter.h"
#include "json/object/type.h"

#include "file/file.h"

#include "user/command.h"

struct File *user_open_file(const char *const name);
Status user_close_file(struct File *const file);

Status user_read_type(struct File *const file, const String *const name, Type *const o_type);
Status user_add_type(struct File *const file, const Type *const type);
Status user_delete_type(struct File *const file, const String *const name);

Status user_apply(struct File *const file, const Command command, struct Iter **iter);