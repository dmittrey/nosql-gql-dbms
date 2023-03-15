#pragma once

#include "utils.h"
#include "table.h"

struct sect_head_t;

struct sect_head_t *sect_head_new();

status_t sect_head_ctor(struct sect_head_t *const, const fileoff_t, FILE *const);
status_t sect_head_dtor(struct sect_head_t *);

status_t sect_head_sync(struct sect_head_t *const);

status_t sect_head_shift_lip(struct sect_head_t *const, const size_t);
status_t sect_head_shift_frp(struct sect_head_t *const, const size_t);

status_t sect_head_cmprs_lip(struct sect_head_t *, size_t);
status_t sect_head_cmprs_frp(struct sect_head_t *);

status_t sect_head_read(struct sect_head_t *, sectoff_t, size_t, void *);
status_t sect_head_write(struct sect_head_t *, sectoff_t, size_t, void *);

fileoff_t sect_head_get_fileoff(const struct sect_head_t *const, const sectoff_t);
sectoff_t sect_head_get_sectoff(const struct sect_head_t *const, const fileoff_t);