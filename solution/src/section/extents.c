#include "section/extents.h"
#include "section/extents_p.h"

Sect_ext *sect_ext_new()
{
    return my_calloc(Sect_ext);
}

Status sect_ext_ctor(Sect_ext *const section, const Fileoff offset, FILE *const filp)
{
    return sect_head_ctor((Sect_head *)section, offset, filp);
}
Status sect_ext_dtor(Sect_ext *section)
{
    return sect_head_dtor((Sect_head *)section);
}

/*
1) Сначала проверим влезет ли запись в секцию
2) Двигаем frp на sizeof(ключ) + sizeof(значение)
3) Записываем ключ
4) Записываем значение
5) Записываем Entity
6) Двигаем lip на sizeof(Entity)
*/
Status sect_ext_write(Sect_ext *const section, const String *const key, const void *const val, const size_t val_sz, Entity *const ent, Sectoff *const save_addr)
{
    if (section->header.free_space < sizeof(Entity) + key->cnt * sizeof(char) + val_sz * sizeof(char))
        return FAILED;

    *save_addr = section->header.lst_itm_ptr;

    // Write key
    DO_OR_FAIL(sect_head_shift_frp((Sect_head *)section, -1 * key->cnt * sizeof(char)));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, section->header.fst_rec_ptr, key->cnt * sizeof(char), key->val));
    ent->key_ptr = section->header.fst_rec_ptr;

    // Write val
    DO_OR_FAIL(sect_head_shift_frp((Sect_head *)section, -1 * val_sz * sizeof(char)));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, section->header.fst_rec_ptr, val_sz * sizeof(char), val));
    ent->val_ptr = section->header.fst_rec_ptr;

    // Write Entity
    DO_OR_FAIL(sect_ext_wrt_itm(section, section->header.lst_itm_ptr, ent));
    DO_OR_FAIL(sect_head_shift_lip((Sect_head *)section, sizeof(Entity)));

    return sect_head_sync((Sect_head *)section);
}

Status sect_ext_read(const Sect_ext *const section, const Sectoff Sectoff, Entity *const o_entity, Json *const o_json)
{
    // Read Entity
    DO_OR_FAIL(sect_ext_rd_itm(section, Sectoff, o_entity));

    // Read key
    char *key = my_malloc_array(char, o_entity->key_size);
    DO_OR_FAIL(sect_head_read((Sect_head *)section, o_entity->key_ptr, o_entity->key_size * sizeof(char), key));

    // Append key to Json
    json_ctor(o_json, o_entity->type, key, o_entity->key_size);
    // Free key ptr
    free(key);

    // Append val to json
    if (o_json->type == TYPE_STRING)
    {
        char *str_val = my_malloc_array(char, o_entity->val_size);
        DO_OR_FAIL(sect_head_read((Sect_head *)section, o_entity->val_ptr, o_entity->val_size * sizeof(char), str_val));

        o_json->value.string_val = string_new();
        string_ctor(o_json->value.string_val, str_val, o_entity->val_size);

        free(str_val);
    }
    else if (o_json->type != TYPE_OBJECT)
    {
        DO_OR_FAIL(sect_head_read((Sect_head *)section, o_entity->val_ptr, o_entity->val_size * sizeof(char), json_val_ptr(o_json)));
    }

    return sect_head_sync((Sect_head *)section);
}

/*
    1) Читаем удаляемую сущность
    2) Если сущность на границе то двигаем указатели
    3) Возвращаем удаляемую сущность при предоставлении буфера
    4) Очищаем байты записей
    5) Очищаем байты сущности
    6) Сжимаем указатели на lip и frp
    7) Синхронизируем header
*/
Status sect_ext_delete(Sect_ext *const section, const Sectoff Sectoff, Entity *o_Entity)
{
    // Read deleted Entity
    Entity *const del_Entity = entity_new();
    DO_OR_FAIL(sect_ext_rd_itm(section, Sectoff, del_Entity));

    // If Entity on border
    if (Sectoff + entity_itm_size(del_Entity) == section->header.lst_itm_ptr)
    {
        DO_OR_FAIL(sect_head_shift_lip(&section->header, -1 * entity_itm_size(del_Entity)));
        DO_OR_FAIL(sect_head_shift_frp(&section->header, entity_rec_sz(del_Entity)));
    }

    // save old Entity
    if (o_Entity)
    {
        memcpy(o_Entity, del_Entity, sizeof(Entity));
    }

    // set null rec fields
    void *temp_zero = memset(my_malloc_array(char, entity_rec_sz(del_Entity)), 0, entity_rec_sz(del_Entity));
    DO_OR_FAIL(sect_head_write((Sect_head *)section, del_Entity->key_ptr - del_Entity->val_size, entity_rec_sz(del_Entity), temp_zero));
    free(temp_zero);

    // set null Entity fields
    DO_OR_FAIL(sect_ext_wrt_itm(section, Sectoff, entity_clear(del_Entity)));

    // shift border by null items
    DO_OR_FAIL(sect_head_cmprs_lip((Sect_head *)section, sizeof(Entity)));
    DO_OR_FAIL(sect_head_cmprs_frp((Sect_head *)section));

    entity_dtor(del_Entity);
    return sect_head_sync((Sect_head *)section);
}

/*
    1) Считываем обновляемую сущность
    2) Дотягиваем размер новой сущности до размера старой при необходимости
    3) Если record находиться на границе
        3.1) Если можем вставить запись в свободное пространство, делаем
    4) Если record находится внутри
        4.1) Если не можем вставить запись на место предыдущей записи
            4.1.1) Вставляем в свободное место внутри секции, если хватает
        4.2) Если можем, то вставляем на место предыдущей записи, прижимая запись к
*/
Status sect_ext_update(Sect_ext *const section, const Sectoff soff, const String *const new_key, const void *const new_val, const size_t new_val_sz, Entity *const new_entity)
{
    // Read old entity
    Entity *old_entity = entity_new();
    DO_OR_FAIL(sect_ext_rd_itm(section, soff, old_entity));

    // Entity init record size promotion
    if (new_entity->init_rec_sz < old_entity->init_rec_sz)
    {
        new_entity->init_rec_sz = old_entity->init_rec_sz;
    }

    // If record is boundary
    if (old_entity->key_ptr - old_entity->val_size == section->header.fst_rec_ptr)
    {
        // If we can insert
        if (section->header.free_space + old_entity->init_rec_sz >= new_entity->init_rec_sz)
        {
            Sectoff prev_lst_itm_ptr = section->header.lst_itm_ptr;
            Sectoff prev_fst_rec_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = soff;
            section->header.fst_rec_ptr += old_entity->init_rec_sz;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            Sectoff save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_key, new_val, new_val_sz, new_entity, &save_addr));

            section->header.lst_itm_ptr = prev_lst_itm_ptr;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            entity_dtor(old_entity);
            return sect_head_sync((Sect_head *)section);
        }
    }
    // If record is inner
    else
    {
        // If we cannot insert record in gap
        if (new_entity->init_rec_sz > old_entity->init_rec_sz)
        {
            // If we can insert record in free space
            if (section->header.free_space >= entity_rec_sz(new_entity))
            {
                sect_ext_delete(section, soff, NULL);

                Sectoff prev_lip = section->header.lst_itm_ptr;
                section->header.lst_itm_ptr = soff;

                Sectoff save_addr;
                DO_OR_FAIL(
                    sect_ext_write(section, new_key, new_val, new_val_sz, new_entity, &save_addr));

                section->header.lst_itm_ptr = prev_lip;
                section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

                entity_dtor(old_entity);
                return sect_head_sync((Sect_head *)section);
            }
        }
        // If we can insert record in gap
        else
        {
            Sectoff prev_lst_itm_ptr = section->header.lst_itm_ptr;
            Sectoff prev_fst_rec_ptr = section->header.fst_rec_ptr;

            section->header.lst_itm_ptr = soff;
            section->header.fst_rec_ptr = old_entity->key_ptr + old_entity->key_size;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            Sectoff save_addr;
            DO_OR_FAIL(
                sect_ext_write(section, new_key, new_val, new_val_sz, new_entity, &save_addr));

            section->header.lst_itm_ptr = prev_lst_itm_ptr;
            section->header.fst_rec_ptr = prev_fst_rec_ptr;
            section->header.free_space = section->header.fst_rec_ptr - section->header.lst_itm_ptr;

            entity_dtor(old_entity);
            return OK;
        }
    }

    entity_dtor(old_entity);
    return FAILED;
}

/*
    1) Загружаем ноды из секции в коллекцию пар Json, Entity
    2) Фильтруем по query(Json должен соответствовать хотя бы одному queryItem)
*/
Status sect_ext_find(Sect_ext *const section, const Query *const q, List_Pair_Json_Entity *const o_list)
{
    DO_OR_FAIL(sect_ext_load(section, o_list));

    if (o_list->count == 0)
        return OK;

    Pair_Json_Entity *cur = o_list->head;
    Pair_Json_Entity *next = o_list->head->next;
    while (next != NULL)
    {
        if (!query_check_or(q, next->f))
        {
            list_Pair_Json_Entity_del_nxt(o_list, cur);
            next = cur->next;
        }
        else
        {
            cur = next;
            next = next->next;
        }
    }

    if (!query_check_or(q, o_list->head->f))
        list_Pair_Json_Entity_del_fst(o_list);

    return OK;
}

/* Private functions */
Status sect_ext_load(const Sect_ext *const section, List_Pair_Json_Entity *const collection)
{
    for (size_t i = sizeof(Sect_head_entity); i < section->header.lst_itm_ptr; i += sizeof(Entity))
    {
        Json *o_Json = json_new();
        Entity *o_Entity = entity_new();
        if (sect_ext_read(section, i, o_Entity, o_Json) == OK)
        {
            Pair_Json_Entity *pair = pair_Json_Entity_new();
            pair_Json_Entity_ctor(pair, o_Json, o_Entity);

            list_Pair_Json_Entity_add(collection, pair);
        }
    }

    return OK;
}

Status sect_ext_wrt_itm(Sect_ext *const section, const Sectoff Sectoff, const Entity *const ent)
{
    return sect_head_write((Sect_head *)section, Sectoff, sizeof(Entity), (void *)ent);
}
Status sect_ext_rd_itm(const Sect_ext *const section, const Sectoff Sectoff, Entity *const o_Entity)
{
    return sect_head_read((Sect_head *)section, Sectoff, sizeof(Entity), o_Entity);
}
int sect_ext_cmp(Sect_ext *this, Sect_ext *other)
{
    return sect_head_cmp((Sect_head *)this, (Sect_head *)other);
}

LIST_DEFINE(Sect_ext, sect_ext_dtor, sect_ext_cmp);