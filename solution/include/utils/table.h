#pragma once

#include <stddef.h>

#define SECTION_SIZE 8192

typedef size_t Fileoff;
typedef size_t Sectoff;

int sectoff_cmp(Sectoff *s1, Sectoff *s2);