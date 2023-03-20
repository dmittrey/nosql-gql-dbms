#pragma once

#include <stddef.h>

#define SECTION_SIZE 8192
// #define SECTION_SIZE 32768

typedef size_t Fileoff;
typedef size_t Sectoff;

int sectoff_cmp(Sectoff *s1, Sectoff *s2);

int fileoff_cmp(Fileoff *s1, Fileoff *s2);