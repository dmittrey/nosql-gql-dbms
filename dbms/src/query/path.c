#include "query/path.h"

void path_dtor(Path *p)
{
    string_dtor(p->name);
}

int path_cmp(Path *this, Path *other)
{
    return string_cmp(this->name, other->name);
}

LIST_DEFINE(Path, path_dtor, path_cmp);