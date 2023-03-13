#pragma once

#include "memory/file/file.h"

struct iter_t;

struct iter_t *iter_new();

void iter_ctor(struct iter_t *const, file_t *const, query_t *const);
void iter_dtor(struct iter_t *);

/* Calc next value in iterator */
void iter_next(struct iter_t *const iter);

/* Check if next value exist */
bool iter_is_avail(struct iter_t *const);

/* Take cur value from iterator */
json_t *iter_get(struct iter_t *const);
