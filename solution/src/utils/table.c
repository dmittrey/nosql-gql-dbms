#include "utils/utils.h"
#include "utils/table.h"

int sectoff_cmp(Sectoff *s1, Sectoff *s2)
{
    return memcmp(s1, s2, sizeof(Sectoff));
}

int fileoff_itm_cmp(Fileoff_itm *s1, Fileoff_itm *s2)
{
    return s1->foff != s2->foff;
}

LIST_DEFINE(Fileoff_itm, free, fileoff_itm_cmp);