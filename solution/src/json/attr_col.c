#include "memory/attribute/attr_col.h"

attr_col_t *attr_col_new()
{
    return calloc(1, sizeof(attr_col_t));
}

void attr_col_dtor(attr_col_t *col)
{
    while (col->f_attr != NULL)
    {
        attr_col_del_fst(col);
    }

    free(col);
}

void attr_col_add_nxt(attr_col_t *const col, attr_t *const attr)
{
    if (col->f_attr == NULL)
    {
        col->f_attr = attr;
        col->l_attr = attr;
    }
    else
    {
        col->l_attr->next = attr;
        col->l_attr = attr;
    }

    col->count += 1;
}

void attr_col_del_fst(attr_col_t *const col)
{
    if (col->f_attr == col->l_attr)
    {
        if (col->f_attr != NULL)
        {
            attr_dtor(col->f_attr);
        }

        col->f_attr = NULL;
        col->l_attr = NULL;
    }
    else
    {
        attr_t *nxt = col->f_attr->next;
        attr_dtor(col->f_attr);
        col->f_attr = nxt;
    }

    col->count -= 1;
}

void attr_col_del_nxt(attr_col_t *const col, attr_t *const attr)
{
    attr_t *nxt = attr->next;
    if (nxt != NULL)
    {
        attr->next = nxt->next;
        attr_dtor(nxt);
    }
    else
    {
        attr->next = NULL;
        col->l_attr = attr;
    }

    col->count -= 1;
}