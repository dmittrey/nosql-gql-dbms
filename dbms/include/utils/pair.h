#pragma once

#include "utils/utils.h"

#define PAIR_DECLARE(F_TYPE, S_TYPE)                                                                              \
    typedef struct Pair_##F_TYPE##_##S_TYPE                                                                       \
    {                                                                                                             \
        F_TYPE *f;                                                                                                \
        S_TYPE *s;                                                                                                \
        struct Pair_##F_TYPE##_##S_TYPE *next;                                                                    \
    } Pair_##F_TYPE##_##S_TYPE;                                                                                   \
    Pair_##F_TYPE##_##S_TYPE *pair_##F_TYPE##_##S_TYPE##_new();                                                   \
    void pair_##F_TYPE##_##S_TYPE##_ctor(Pair_##F_TYPE##_##S_TYPE *const pair, F_TYPE *const f, S_TYPE *const s); \
    void pair_##F_TYPE##_##S_TYPE##_dtor(Pair_##F_TYPE##_##S_TYPE *pair);                                         \
    int pair_##F_TYPE##_##S_TYPE##_cmp(Pair_##F_TYPE##_##S_TYPE *t, Pair_##F_TYPE##_##S_TYPE *o);

#define PAIR_DEFINE(F_TYPE, S_TYPE, F_T_DTOR, F_T_CMP, S_T_DTOR, S_T_CMP)                                        \
    Pair_##F_TYPE##_##S_TYPE *pair_##F_TYPE##_##S_TYPE##_new()                                                   \
    {                                                                                                            \
        return calloc(1, sizeof(Pair_##F_TYPE##_##S_TYPE));                                                      \
    }                                                                                                            \
    void pair_##F_TYPE##_##S_TYPE##_ctor(Pair_##F_TYPE##_##S_TYPE *const pair, F_TYPE *const f, S_TYPE *const s) \
    {                                                                                                            \
        pair->f = f;                                                                                             \
        pair->s = s;                                                                                             \
    }                                                                                                            \
    void pair_##F_TYPE##_##S_TYPE##_dtor(Pair_##F_TYPE##_##S_TYPE *pair)                                         \
    {                                                                                                            \
        F_T_DTOR(pair->f);                                                                                       \
        S_T_DTOR(pair->s);                                                                                       \
        free(pair);                                                                                              \
    }                                                                                                            \
    int pair_##F_TYPE##_##S_TYPE##_cmp(Pair_##F_TYPE##_##S_TYPE *this, Pair_##F_TYPE##_##S_TYPE *other)          \
    {                                                                                                            \
        int f_cmp = F_T_CMP(this->f, other->f);                                                                  \
        if (f_cmp != 0)                                                                                          \
            return f_cmp;                                                                                        \
        int s_cmp = S_T_CMP(this->s, other->s);                                                                  \
        return s_cmp;                                                                                            \
    }