#pragma once

#include "page_p.h"
#include "utils.h"

struct extent_t;

PerformStatus extent_insert(struct extent_t *);
PerformStatus extent_update(struct extent_t *);
PerformStatus extent_remove(struct extent_t *);
PerformStatus extent_read(struct extent_t *);