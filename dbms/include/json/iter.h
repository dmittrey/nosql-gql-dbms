#pragma once

#include "file/file.h"

struct Iter;

struct Iter *iter_new();

void iter_ctor(struct Iter *const, struct File *const, Query *const);
void iter_dtor(struct Iter *);

/* Calc next value in iterator */
void iter_next(struct Iter *const iter);

/* Check if next value exist */
bool iter_is_avail(struct Iter *const);

/* Take cur value from iterator */
Json *iter_get_json(struct Iter *const iter);
Entity *iter_get_entity(struct Iter *const iter);
