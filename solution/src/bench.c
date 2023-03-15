#include <time.h>

#include "user/user.h"

static const char *test_file_name = "bench.bin";

void bench_write()
{
    /*
    "info" : {
    "city" : {
        "location" : "Moscow",
        "amount" : "50000"
    },
    "flag": true
    }
    */

    struct timespec ts;

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    long ins_cnt = 0;
    timespec_get(&ts, TIME_UTC);
    long cur_ts = ts.tv_nsec;

    fileoff_t wrt_addr;
    for (size_t i = 0; i < 500; i++)
    {
        for (size_t i = 0; i < 200; i++)
        {
            user_write(file, info_json);

            timespec_get(&ts, TIME_UTC);
            cur_ts = ts.tv_nsec;
        }

        timespec_get(&ts, TIME_UTC);
        long delta = ts.tv_nsec - cur_ts;
        cur_ts = ts.tv_nsec;

        printf("%ld, %ld\n", ++ins_cnt, delta);
    }
}

void bench_read()
{
    /*
    "info" : {
    "city" : {
        "location" : "Moscow",
        "amount" : "50000"
    },
    "flag": true
    }
    */

    struct timespec ts;

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    long ins_cnt = 0;

    fileoff_t wrt_addr;
    for (size_t i = 0; i < 200; i++)
    {
        for (size_t i = 0; i < 50; i++)
        {
            user_write(file, info_json);
            user_write(file, location_json);
            user_write(file, flag_json);
            ins_cnt += 3;
        }

        timespec_get(&ts, TIME_UTC);
        long cur_ts = ts.tv_nsec;

        struct iter_t *iter = user_read(file, query);
        while (iter_is_avail(iter))
        {
            iter_next(iter);
        }

        timespec_get(&ts, TIME_UTC);
        long delta = ts.tv_nsec - cur_ts;
        printf("%ld, %ld\n", ins_cnt, delta);
    }
}

void bench_delete()
{
    /*
    "info" : {
    "city" : {
        "location" : "Moscow",
        "amount" : "50000"
    },
    "flag": true
    }
    */

    struct timespec ts;

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    long ins_cnt = 0;

    // for (size_t i = 0; i < 200; i++)
    // {
        // for (size_t i = 0; i < 50; i++)
        // {
            user_write(file, info_json);
            user_write(file, location_json);
            user_write(file, flag_json);
            ins_cnt += 3;
        // }

        timespec_get(&ts, TIME_UTC);
        long cur_ts = ts.tv_nsec;

        user_delete(file, query);

        timespec_get(&ts, TIME_UTC);
        long delta = ts.tv_nsec - cur_ts;
        printf("%ld, %ld\n", ins_cnt, delta);
    // }
}