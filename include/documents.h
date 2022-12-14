#pragma once

#include "page_p.h"
#include "utils.h"

struct documents_t;

enum PerformStatus documents_insert(struct documents_t *);
enum PerformStatus documents_update(struct documents_t *);
enum PerformStatus documents_remove(struct documents_t *);
enum PerformStatus documents_read(struct documents_t *);