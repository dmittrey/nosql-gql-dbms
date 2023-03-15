#include "memory/section/types_p.h"
#include "memory/section/types.h"

#include "physical/section/header.h"

static status_t sect_type_wrt(sect_type_t *const section, const sectoff_t sectoff, void *data, size_t size);
static status_t sect_type_rd(sect_type_t *const section, const sectoff_t sectoff, size_t size, void *o_data);

static status_t reduce_lst_itm_ptr_emt(sect_type_t *section);
static status_t reduce_fst_rec_ptr_emt(sect_type_t *section);

sect_type_t *sect_type_new()
{
    return calloc(1, sizeof(sect_type_t));
}

status_t sect_type_ctor(sect_type_t *const section, const fileoff_t fileoff, FILE *const filp)
{
    return sect_head_ctor((sect_head_t *)section, fileoff, filp);
}
void sect_type_dtor(sect_type_t *section)
{
    section->next = NULL;
    sect_head_dtor((sect_head_t *)section);
}

/*
1) Проверить можно ли разместить тип
2) Если можем, то размещаем
*/
status_t sect_type_write(sect_type_t *const section, const type_t *const type, sectoff_t *const o_wrt_soff)
{
    size_t attrs_size = 0;
    attr_t *cur = type->attr_list->head;
    while (cur != NULL)
    {
        attrs_size += attr_entity_sz(cur->name->cnt);
        cur = cur->next;
    }

    if (section->header.free_space < type_entity_sz(type->name->cnt) + attrs_size)
        return FAILED;

    type_entity_t *entity = type_entity_new();
    attr_entity_t *cur_entity = attr_entity_new();

    DO_OR_FAIL(sect_head_shift_frp(&section->header, -1 * type->name->cnt));
    DO_OR_FAIL(sect_type_wrt(section, section->header.fst_rec_ptr, type->name->val, type->name->cnt));
    entity->name_size = type->name->cnt;
    entity->name_ptr = section->header.fst_rec_ptr;
    entity->attr_cnt = type->attr_list->count;

    DO_OR_FAIL(sect_type_wrt(section, section->header.lst_itm_ptr, entity, sizeof(type_entity_t)));
    *o_wrt_soff = section->header.lst_itm_ptr;
    DO_OR_FAIL(sect_head_shift_lip(&section->header, sizeof(type_entity_t)));

    cur = type->attr_list->head;
    while (cur != NULL)
    {
        attr_entity_ctor(cur_entity, cur);

        DO_OR_FAIL(sect_head_shift_frp(&section->header, -1 * cur->name->cnt));
        DO_OR_FAIL(sect_type_wrt(section, section->header.fst_rec_ptr, cur->name->val, cur->name->cnt));
        cur_entity->name_size = cur->name->cnt;
        cur_entity->name_ptr = section->header.fst_rec_ptr;
        cur_entity->attr_type = cur->type;

        DO_OR_FAIL(sect_type_wrt(section, section->header.lst_itm_ptr, cur_entity, sizeof(type_entity_t)));
        DO_OR_FAIL(sect_head_shift_lip(&section->header, sizeof(type_entity_t)));

        cur = cur->next;
    }

    type_entity_dtor(entity);
    attr_entity_dtor(cur_entity);
    return OK;
}

status_t sect_type_delete(sect_type_t *const section, const sectoff_t del_soff)
{
    type_t *t = type_new();
    type_entity_t *t_ent = type_entity_new();
    sect_type_read(section, del_soff, t, t_ent);

    size_t itm_size = sizeof(type_entity_t) + t->attr_list->count * sizeof(attr_entity_t);

    size_t rec_size = t->name->cnt * sizeof(char);
    attr_t *cur_atr = t->attr_list->head;
    while (cur_atr != NULL)
    {
        rec_size += cur_atr->name->cnt * sizeof(char);
        cur_atr = cur_atr->next;
    }

    if (del_soff + sizeof(type_entity_t) + t->attr_list->count * sizeof(attr_entity_t) == section->header.lst_itm_ptr)
    {
        DO_OR_FAIL(sect_head_shift_lip(&section->header, -1 * itm_size));
        DO_OR_FAIL(sect_head_shift_frp(&section->header, rec_size));
    }

    // Set null rec fields
    void *itm_zero = memset(my_malloc_array(char, itm_size), 0, itm_size);
    void *rec_zero = memset(my_malloc_array(char, rec_size), 0, rec_size);
    sect_type_wrt(section, t_ent->name_ptr + t->name->cnt - rec_size, rec_zero, rec_size);
    sect_type_wrt(section, del_soff, itm_zero, itm_size);

    // Shift border by null items
    reduce_lst_itm_ptr_emt(section);
    reduce_fst_rec_ptr_emt(section);

    type_dtor(t);
    type_entity_dtor(t_ent);

    free(itm_zero);
    free(rec_zero);
    return OK;
}

status_t sect_type_read(sect_type_t *const section, sectoff_t sctoff, type_t *const o_type, type_entity_t *const o_type_ent)
{
    sect_type_rd_ent(section, sctoff, o_type_ent);
    char *t_name_c = my_malloc_array(char, o_type_ent->name_size);
    sect_type_rd(section, o_type_ent->name_ptr, o_type_ent->name_size, t_name_c);

    string_t *t_name = string_new();
    string_ctor(t_name, t_name_c, o_type_ent->name_size);
    free(t_name_c);
    list_attr_t *atr_list = list_attr_t_new();
    type_ctor_foff(o_type, t_name, atr_list, sect_head_get_fileoff(&section->header, sctoff));
    sctoff += sizeof(type_entity_t);

    attr_entity_t *a_ent = attr_entity_new();
    for (size_t i = 0; i < o_type_ent->attr_cnt; i++)
    {
        sect_type_rd_atr(section, sctoff + i * sizeof(attr_entity_t), a_ent);
        char *a_name_c = my_malloc_array(char, a_ent->name_size);
        sect_type_rd(section, a_ent->name_ptr, a_ent->name_size, a_name_c);

        attr_t *a = attr_new();
        string_t *a_name = string_new();
        string_ctor(a_name, a_name_c, a_ent->name_size);
        free(a_name_c);
        attr_ctor(a, a_name, a_ent->attr_type);

        list_attr_t_add(o_type->attr_list, a);
    }
    attr_entity_dtor(a_ent);

    return OK;
}

status_t sect_type_find(sect_type_t *const section, const string_t *const type_name, type_t *const o_type)
{
    list_type_t *type_list = list_type_t_new();
    sect_type_load(section, type_list);

    while (type_list->head != NULL)
    {
        if (string_cmp(type_list->head->name, type_name) == 0)
        {
            type_cpy(o_type, type_list->head);
            break;
        }
        else
        {
            list_type_t_del_fst(type_list);
        }
    }

    list_type_t_dtor(type_list);

    return OK;
}

status_t sect_types_sync(sect_type_t *const section)
{
    return sect_head_sync(&section->header);
}

status_t sect_type_load(sect_type_t *const section, list_type_t *const o_type_list)
{
    sectoff_t cur_sctoff = sizeof(sect_head_entity_t);

    while (cur_sctoff < section->header.lst_itm_ptr)
    {
        type_t *t = type_new();
        type_entity_t *t_ent = type_entity_new();

        sect_type_read(section, cur_sctoff, t, t_ent);
        type_entity_dtor(t_ent);

        list_type_t_add(o_type_list, t);
        cur_sctoff += sizeof(type_entity_t) + t->attr_list->count * sizeof(attr_entity_t);
    }

    return OK;
}

status_t sect_type_rd_ent(sect_type_t *const section, const sectoff_t sectoff, type_entity_t *const o_ent)
{
    SAVE_FILP(section->header.filp, {
        RA_FREAD_OR_FAIL(o_ent, sizeof(type_entity_t), sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}

status_t sect_type_rd_atr(sect_type_t *const section, const sectoff_t sectoff, attr_entity_t *const o_atr)
{
    SAVE_FILP(section->header.filp, {
        RA_FREAD_OR_FAIL(o_atr, sizeof(attr_entity_t), sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}

static status_t sect_type_wrt(sect_type_t *const section, const sectoff_t sectoff, void *data, size_t size)
{
    SAVE_FILP(section->header.filp, {
        RA_FWRITE_OR_FAIL(data, size, sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}

static status_t sect_type_rd(sect_type_t *const section, const sectoff_t sectoff, size_t size, void *o_data)
{
    SAVE_FILP(section->header.filp, {
        RA_FREAD_OR_FAIL(o_data, size, sect_head_get_fileoff(&section->header, sectoff), section->header.filp);
    });

    return OK;
}

static status_t reduce_lst_itm_ptr_emt(sect_type_t *section)
{
    type_entity_t *lst_entity = type_entity_new();
    type_entity_t *rd_entity = type_entity_new();

    sect_type_rd_ent(section, section->header.lst_itm_ptr - sizeof(type_entity_t), rd_entity);

    while (section->header.lst_itm_ptr != sizeof(sect_head_entity_t) && memcmp(lst_entity, rd_entity, sizeof(type_entity_t)) == 0)
    {
        section->header.lst_itm_ptr -= sizeof(type_entity_t);
        section->header.free_space += sizeof(type_entity_t);

        sect_type_rd_ent(section, section->header.lst_itm_ptr - sizeof(type_entity_t), rd_entity);
    }

    type_entity_dtor(lst_entity);
    type_entity_dtor(rd_entity);

    return sect_head_sync((sect_head_t *)section);
}
static status_t reduce_fst_rec_ptr_emt(sect_type_t *section)
{
    char chr;
    while (true)
    {
        sect_type_rd(section, section->header.fst_rec_ptr, 1, &chr);

        if (section->header.fst_rec_ptr == SECTION_SIZE || chr != 0)
            break;

        section->header.fst_rec_ptr += 1;
        section->header.free_space += 1;
    }

    return sect_head_sync((sect_head_t *)section);
}