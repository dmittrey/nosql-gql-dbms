#pragma once

#include "section/extents_p.h"
#include "section/types_p.h"

#include "file/header.h"
#include "file/header_p.h"

typedef struct File
{
    File_head header;
    List_Sect_ext *f_extent;
    List_Sect_types *f_types;
} File;

Status file_add_sect_ext(File *const file, struct Sect_ext *r_extents);
Status file_add_sect_types(File *const file, struct Sect_types *r_types);

Sect_ext *get_sect_ext(const File *const file, Fileoff fileoff);
Sect_types *get_sect_types(const File *const file, Fileoff fileoff);