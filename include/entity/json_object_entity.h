#pragma once

#include "table.h"

#include "json/json_object.h"

typedef struct
{
    uint64_t key_size;   // Размер строки ключа
    sectoff_t key_ptr;   // Указатель на строку в секции
    fileoff_t value_ptr; // Указатель на запись в базе
} kv_entity;

/*
JSON obj serialization:

| Nodes body  |          |                |               |               |     |
| ----------- | -------- | -------------- |-------------- |-------------- | --- |
| Content     | attr_cnt | attr1_key_size | attr1_key_ptr | attr1_val_ptr | ... |
| Size(bytes) | 8        | 8              | 8             | 8             |     |
*/