#include "utils/utils.h"
#include "utils/table.h"

int sectoff_cmp(Sectoff *s1, Sectoff *s2)
{
    return memcmp(s1, s2, sizeof(Sectoff));
}

int fileoff_cmp(Fileoff *s1, Fileoff *s2)
{
    return memcmp(s1, s2, sizeof(Fileoff));
}