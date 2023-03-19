#include "utils/pair.h"

#include "section/types.h"
#include "section/types_p.h"

Sect_types *sect_types_new()
{
    return my_calloc(Sect_types);
}

Status sect_types_ctor(Sect_types *const section, const Fileoff fileoff, FILE *const filp)
{
    return sect_head_ctor((Sect_head *)section, fileoff, filp);
}
Status sect_types_dtor(Sect_types *section)
{
    return sect_head_dtor((Sect_head *)section);
}

Status sect_types_write(struct Sect_types *const section, const Type *const type, Sectoff *const o_wrt_soff)
{
    // Check if free space for type exist
    if (section->header.free_space < type_ph_sz(type))
        return FAILED;

    // Write type name
    DO_OR_FAIL(sect_head_shift_frp(&section->header, -1 * type->name->cnt));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, section->header.fst_rec_ptr, type->name->cnt, type->name->val));

    // Initialize type entity
    Type_entity *t_ent = type_entity_new();
    t_ent->attr_cnt = type->attr_list->count;
    t_ent->name_ptr = section->header.fst_rec_ptr;
    t_ent->name_size = type->name->cnt;

    // Write type entity
    *o_wrt_soff = section->header.lst_itm_ptr;
    DO_OR_FAIL(sect_types_wrt_type(section, section->header.lst_itm_ptr, t_ent));
    DO_OR_FAIL(sect_head_shift_lip((Sect_head *)section, sizeof(Type_entity)));

    Attr_entity *cur_a_ent = attr_entity_new();
    Attr *cur_a = type->attr_list->head;
    while (cur_a != NULL)
    {
        // Write attr name
        DO_OR_FAIL(sect_head_shift_frp(&section->header, -1 * cur_a->name->cnt));
        DO_OR_FAIL(sect_head_write((Sect_head *)section, section->header.fst_rec_ptr, cur_a->name->cnt, cur_a->name->val));

        // Initialize attr entity
        cur_a_ent->name_size = cur_a->name->cnt;
        cur_a_ent->name_ptr = section->header.fst_rec_ptr;
        cur_a_ent->attr_type = cur_a->type;

        // Write attr entity
        DO_OR_FAIL(sect_types_wrt_atr(section, section->header.lst_itm_ptr, cur_a_ent));
        DO_OR_FAIL(sect_head_shift_lip((Sect_head *)section, sizeof(Attr_entity)));

        cur_a = cur_a->next;
    }

    type_entity_dtor(t_ent);
    attr_entity_dtor(cur_a_ent);
    return OK;
}

Status sect_types_delete(struct Sect_types *const section, const Sectoff del_soff)
{
    // Read type entity
    Type_entity *t_ent = type_entity_new();
    DO_OR_FAIL(sect_types_rd_type(section, del_soff, t_ent));

    // Read last attr entity
    Attr_entity *lst_atr_ent = attr_entity_new();
    DO_OR_FAIL(sect_types_rd_atr(section, del_soff + sizeof(Type_entity) + (t_ent->attr_cnt - 1) * sizeof(Attr_entity), lst_atr_ent));

    size_t itm_sz = sizeof(Type_entity) + t_ent->attr_cnt * sizeof(Attr_entity);
    /*
    | lst_name | ... | type_name |
    /\                           /\
    |                            |
    lst_atr_ent->name_ptr        (t_ent->name_ptr + t_ent->name_size)
    */
    size_t rec_sz = (t_ent->name_ptr + t_ent->name_size) - lst_atr_ent->name_ptr;

    // Set null itm space
    void *itm_zr = calloc(itm_sz, sizeof(char));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, del_soff, itm_sz, itm_zr));
    free(itm_zr);

    // Set null rec space
    void *rec_zr = calloc(rec_sz, sizeof(char));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, lst_atr_ent->name_ptr, rec_sz, rec_zr));
    free(rec_zr);

    // Shift border by null items
    DO_OR_FAIL(sect_head_cmprs_lip((Sect_head *)section, sizeof(Attr_entity)));
    DO_OR_FAIL(sect_head_cmprs_frp((Sect_head *)section));

    type_entity_dtor(t_ent);
    attr_entity_dtor(lst_atr_ent);
    return OK;
}

Status sect_types_read(struct Sect_types *const section, Sectoff soff, Type *const o_type, Type_entity *const o_type_ent, size_t *o_ph_sz)
{
    *o_ph_sz = soff;

    // Read type entity
    DO_OR_FAIL(sect_types_rd_type(section, soff, o_type_ent));

    // Read type name
    char *t_name_c = my_malloc_array(char, o_type_ent->name_size);
    DO_OR_FAIL(sect_head_read((Sect_head *)section, o_type_ent->name_ptr, o_type_ent->name_size, t_name_c));

    String *t_name = string_new();
    List_Attr *atr_list = list_Attr_new();
    string_ctor(t_name, t_name_c, o_type_ent->name_size);
    type_ctor(o_type, t_name, atr_list);

    Attr_entity *a_ent = attr_entity_new();
    soff += sizeof(Type_entity);
    for (size_t i = 0; i < o_type_ent->attr_cnt; i++)
    {
        DO_OR_FAIL(sect_types_rd_atr(section, soff, a_ent));
        char *a_name_c = my_malloc_array(char, a_ent->name_size);
        DO_OR_FAIL(sect_head_read((Sect_head *)section, a_ent->name_ptr, a_ent->name_size, a_name_c));

        Attr *a = attr_new();
        String *a_name = string_new();
        string_ctor(a_name, a_name_c, a_ent->name_size);
        attr_ctor(a, a_name, a_ent->attr_type);
        free(a_name_c);

        type_add_atr(o_type, a);
        soff += sizeof(Attr_entity);
    }
    *o_ph_sz = soff - *o_ph_sz;

    attr_entity_dtor(a_ent);
    free(t_name_c);
    return OK;
}

Status sect_types_load(struct Sect_types *const section, List_Pair_Sectoff_Type *const o_list)
{
    Sectoff soff = sizeof(Sect_head_entity);
    size_t ph_sz;

    Type_entity *t_ent = type_entity_new();
    while (soff < section->header.lst_itm_ptr)
    {
        Type *t = type_new();
        Sectoff *t_soff = my_malloc(Sectoff);
        *t_soff = soff;

        DO_OR_FAIL(sect_types_read(section, soff, t, t_ent, &ph_sz));

        Pair_Sectoff_Type *pair = pair_Sectoff_Type_new();
        pair_Sectoff_Type_ctor(pair, t_soff, t);

        list_Pair_Sectoff_Type_add(o_list, pair);

        soff += ph_sz;
    }
    type_entity_dtor(t_ent);

    return OK;
}

Status sect_types_find(struct Sect_types *const section, const String *const type_name, Type *const o_type, Sectoff *adr)
{
    List_Pair_Sectoff_Type *type_list = list_Pair_Sectoff_Type_new();
    DO_OR_FAIL(sect_types_load(section, type_list));

    while (type_list->head != NULL)
    {
        if (string_cmp(type_list->head->s->name, type_name) == 0)
        {
            type_cpy(o_type, type_list->head->s);
            *adr = *type_list->head->f;

            list_Pair_Sectoff_Type_dtor(type_list);
            return OK;
        }
        else
        {
            list_Pair_Sectoff_Type_del_fst(type_list);
        }
    }

    list_Pair_Sectoff_Type_dtor(type_list);

    return FAILED;
}

/* Private functions */
Status sect_types_rd_type(Sect_types *const section, const Sectoff sectoff, Type_entity *const o_ent)
{
    return sect_head_read((Sect_head *)section, sectoff, sizeof(Type_entity), o_ent);
}
Status sect_types_rd_atr(Sect_types *const section, const Sectoff sectoff, Attr_entity *const o_atr)
{
    return sect_head_read((Sect_head *)section, sectoff, sizeof(Attr_entity), o_atr);
}
Status sect_types_wrt_type(Sect_types *const section, const Sectoff sectoff, const Type_entity *const o_ent)
{
    return sect_head_write((Sect_head *)section, sectoff, sizeof(Type_entity), o_ent);
}
Status sect_types_wrt_atr(Sect_types *const section, const Sectoff sectoff, const Attr_entity *const o_atr)
{
    return sect_head_write((Sect_head *)section, sectoff, sizeof(Attr_entity), o_atr);
}
int sect_types_cmp(Sect_types *this, Sect_types *other)
{
    return sect_head_cmp((Sect_head *)this, (Sect_head *)other);
}

PAIR_DEFINE(Sectoff, Type, free, sectoff_cmp, type_dtor, type_cmp);
LIST_DEFINE(Pair_Sectoff_Type, pair_Sectoff_Type_dtor, pair_Sectoff_Type_cmp);

LIST_DEFINE(Sect_types, sect_types_dtor, sect_types_cmp);