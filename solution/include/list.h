#pragma once

#define LIST_DECLARE(TYPE)                                               \
    typedef struct                                                       \
    {                                                                    \
        TYPE *head;                                                      \
        TYPE *tail;                                                      \
        size_t count;                                                    \
    } list_##TYPE;                                                       \
                                                                         \
    list_##TYPE *list_##TYPE##_new();                                    \
    void list_##TYPE##_dtor(list_##TYPE *collection);                    \
    void list_##TYPE##_add(list_##TYPE *const col, TYPE *const ent);     \
    void list_##TYPE##_del_fst(list_##TYPE *const col);                  \
    void list_##TYPE##_del_nxt(list_##TYPE *const col, TYPE *const ent); \
    bool list_##TYPE##_add_lk_set(list_##TYPE *const col, TYPE *const ent);

#define LIST_DEFINE(TYPE, DTOR_FUNC, CMP_FUNC)                                    \
    inline list_##TYPE *list_##TYPE##_new()                                       \
    {                                                                             \
        return calloc(1, sizeof(list_##TYPE));                                    \
    }                                                                             \
                                                                                  \
    inline void list_##TYPE##_dtor(list_##TYPE *collection)                       \
    {                                                                             \
        while (collection->head != NULL)                                          \
        {                                                                         \
            TYPE *cur = collection->head;                                         \
            collection->head = collection->head->next;                            \
            DTOR_FUNC(cur);                                                       \
        }                                                                         \
                                                                                  \
        free(collection);                                                         \
    }                                                                             \
                                                                                  \
    inline void list_##TYPE##_add(list_##TYPE *const col, TYPE *const ent)        \
    {                                                                             \
        if (col->head == NULL)                                                    \
        {                                                                         \
            col->head = ent;                                                      \
            col->tail = ent;                                                      \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            col->tail->next = ent;                                                \
            col->tail = ent;                                                      \
        }                                                                         \
                                                                                  \
        col->count += 1;                                                          \
    }                                                                             \
                                                                                  \
    inline void list_##TYPE##_del_fst(list_##TYPE *const col)                     \
    {                                                                             \
        if (col->count > 1)                                                       \
        {                                                                         \
            TYPE *nxt = col->head->next;                                          \
            DTOR_FUNC(col->head);                                                 \
            col->head = nxt;                                                      \
        }                                                                         \
        else if (col->count == 1)                                                 \
        {                                                                         \
            col->head = NULL;                                                     \
            col->tail = NULL;                                                     \
        }                                                                         \
                                                                                  \
        col->count -= 1;                                                          \
    }                                                                             \
                                                                                  \
    inline void list_##TYPE##_del_nxt(list_##TYPE *const col, TYPE *const ent)    \
    {                                                                             \
        TYPE *nxt = ent->next;                                                    \
                                                                                  \
        if (col->tail == nxt)                                                     \
        {                                                                         \
            col->tail = ent;                                                      \
        }                                                                         \
                                                                                  \
        if (nxt != NULL)                                                          \
        {                                                                         \
            ent->next = nxt->next;                                                \
            DTOR_FUNC(nxt);                                                       \
        }                                                                         \
    }                                                                             \
                                                                                  \
    inline bool list_##TYPE##_add_lk_set(list_##TYPE *const col, TYPE *const ent) \
    {                                                                             \
        if (col->head == NULL)                                                    \
        {                                                                         \
            col->head = ent;                                                      \
            col->tail = ent;                                                      \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            TYPE *cur = col->head;                                                \
            while (cur != NULL)                                                   \
            {                                                                     \
                if (CMP_FUNC(cur, ent) == 0)                                      \
                    return false;                                                 \
                cur = cur->next;                                                  \
            }                                                                     \
            col->tail->next = ent;                                                \
            col->tail = ent;                                                      \
        }                                                                         \
        col->count += 1;                                                          \
        return true;                                                              \
    }