#include <time.h>

#include "user/user.h"

#include "file/file_p.h"

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

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    long ins_cnt = 0;
    timespec_get(&ts, TIME_UTC);
    long cur_ts = ts.tv_nsec;

    TYPE_INIT(V_type, "V");
    user_add_type(file, V_type);

    Fileoff wrt_addr;
    for (size_t i = 0; i < 500; i++)
    {
        for (size_t i = 0; i < 200; i++)
        {
            user_write(file, info_json, V_type->name);
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

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    List_Path *flag_q_key_path_list = list_Path_new();

    STR_INIT(flag_q_key, "flag");
    PATH(flag_q_key);

    list_Path_add(flag_q_key_path_list, flag_q_key_path);

    Query_item *flag_q_itm = query_item_new();
    query_item_ctor(flag_q_itm, TYPE_INT32, flag_q_key_path_list);
    flag_q_itm->query_val.bool_val = true;

    STR_INIT(q_name, "K");
    Query *query = query_new();
    query_ctor(query, q_name);
    query_item_add(query, flag_q_itm);

    long ins_cnt = 0;

    TYPE_INIT(K_type, "K");
    user_add_type(file, K_type);

    TYPE_INIT(T_type, "T");
    user_add_type(file, T_type);

    TYPE_INIT(V_type, "V");
    user_add_type(file, V_type);

    Fileoff wrt_addr;
    for (size_t i = 0; i < 200; i++)
    {
        for (size_t i = 0; i < 50; i++)
        {
            user_write(file, info_json, K_type->name);
            user_write(file, location_json, K_type->name);
            user_write(file, flag_json, K_type->name);
            ins_cnt += 3;
        }

        timespec_get(&ts, TIME_UTC);
        long cur_ts = ts.tv_nsec;

        struct Iter *iter = user_read(file, query);
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

    "glossary": {
        "sort": "SGML",
        "glossterm": "Standard Generalized Markup Language",
        "acronym": "SGML",
        "abbrev": "ISO 8879:1986"
    }
    */

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 5);
    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);
    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    STR_INIT(sort_str, "SGML");
    JSON_VALUE_INIT(TYPE_STRING, sort, "sort", sort_str);
    STR_INIT(glossterm_str, "Standard Generalized Markup Language");
    JSON_VALUE_INIT(TYPE_STRING, glossterm, "glossterm", glossterm_str);
    STR_INIT(acronym_str, "SGML");
    JSON_VALUE_INIT(TYPE_STRING, acronym, "acronym", acronym_str);
    STR_INIT(abbrev_str, "ISO 8879:1986");
    JSON_VALUE_INIT(TYPE_STRING, abbrev, "abbrev", abbrev_str);

    JSON_VALUE_INIT(TYPE_OBJECT, glossary, "glossary", NULL);
    json_add_son(glossary, sort);
    json_add_son(glossary, glossterm);
    json_add_son(glossary, acronym);
    json_add_son(glossary, abbrev);

    File *file = user_open_file(test_file_name);

    TYPE_INIT(V_type, "V");
    user_add_type(file, V_type);

    List_Path *flag_q_key_path_list = list_Path_new();

    STR_INIT(flag_q_key, "flag");
    PATH(flag_q_key);

    list_Path_add(flag_q_key_path_list, flag_q_key_path);

    Query_item *flag_q_itm = query_item_new();
    query_item_ctor(flag_q_itm, TYPE_INT32, flag_q_key_path_list);
    flag_q_itm->query_val.bool_val = true;

    Query *amount_query = query_new();
    query_item_add(amount_query, flag_q_itm);

    List_Path *sort_q_key_path_list = list_Path_new();

    STR_INIT(sort_q_key, "sort");
    PATH(sort_q_key);

    list_Path_add(sort_q_key_path_list, sort_q_key_path);

    Query_item *sort_q_itm = query_item_new();
    STR_INIT(sgml, "SGML");
    query_item_ctor(sort_q_itm, TYPE_STRING, sort_q_key_path_list);
    sort_q_itm->query_val.string_val = sgml;

    Query *sort_query = query_new();
    query_item_add(sort_query, sort_q_itm);

    long ins_cnt = 0;

    for (size_t i = 0; i < 30; i++)
    {
        for (size_t j = 0; j < 50 * i; j++)
        {
            if (user_write(file, info_json, V_type->name) == FAILED)
                printf("Failed");
            if (user_write(file, glossary, V_type->name) == FAILED)
                printf("Failed");
        }
        ins_cnt = 50 * i;

        clock_t start = clock();

        user_delete(file, amount_query);

        clock_t end = clock();
        double elapsed = (double)(end - start) * 10;
        printf("%ld, %f \t", ins_cnt, elapsed);

        user_delete(file, sort_query);
        printf("\n");
    }

    type_dtor(V_type);
    query_dtor(amount_query);
    query_dtor(sort_query);
    json_dtor(info_json);
    json_dtor(glossary);
    user_close_file(file);
}

void bench_update()
{
    /*
    "info" : {
        "city" : {
            "location" : "Moscow",
            "amount" : "50000"
        },
        "flag": true
    }

    "glossary": {
        "sort": "SGML",
        "glossterm": "Standard Generalized Markup Language",
        "acronym": "SGML",
        "abbrev": "ISO 8879:1986"
    }
    */

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 5);
    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);
    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    STR_INIT(sort_str, "SGML");
    JSON_VALUE_INIT(TYPE_STRING, sort, "sort", sort_str);
    STR_INIT(glossterm_str, "Standard Generalized Markup Language");
    JSON_VALUE_INIT(TYPE_STRING, glossterm, "glossterm", glossterm_str);
    STR_INIT(acronym_str, "SGML");
    JSON_VALUE_INIT(TYPE_STRING, acronym, "acronym", acronym_str);
    STR_INIT(abbrev_str, "ISO 8879:1986");
    JSON_VALUE_INIT(TYPE_STRING, abbrev, "abbrev", abbrev_str);

    JSON_VALUE_INIT(TYPE_OBJECT, glossary, "glossary", NULL);
    json_add_son(glossary, sort);
    json_add_son(glossary, glossterm);
    json_add_son(glossary, acronym);
    json_add_son(glossary, abbrev);

    File *file = user_open_file(test_file_name);

    TYPE_INIT(V_type, "V");
    user_add_type(file, V_type);

    List_Path *flag_q_key_path_list = list_Path_new();

    STR_INIT(flag_q_key, "flag");
    PATH(flag_q_key);

    list_Path_add(flag_q_key_path_list, flag_q_key_path);

    Query_item *flag_q_itm = query_item_new();
    query_item_ctor(flag_q_itm, TYPE_INT32, flag_q_key_path_list);
    flag_q_itm->query_val.bool_val = true;

    Query *amount_query = query_new();
    query_item_add(amount_query, flag_q_itm);

    List_Path *sort_q_key_path_list = list_Path_new();

    STR_INIT(sort_q_key, "sort");
    PATH(sort_q_key);

    list_Path_add(sort_q_key_path_list, sort_q_key_path);

    Query_item *sort_q_itm = query_item_new();
    STR_INIT(sgml, "SGML");
    query_item_ctor(sort_q_itm, TYPE_STRING, sort_q_key_path_list);
    sort_q_itm->query_val.string_val = sgml;

    Query *sort_query = query_new();
    query_item_add(sort_query, sort_q_itm);

    for (size_t i = 1; i < 51; i++)
    {
        long ins_cnt = 0;
        for (size_t j = 0; j < 200 * i; j++)
        {
            if (user_write(file, info_json, V_type->name) == OK)
            {
                ins_cnt++;
            }
            user_write(file, glossary, V_type->name);
        }

        clock_t start = clock();

        user_update(file, amount_query, glossary);

        clock_t end = clock();
        double elapsed = (double)(end - start) * 10;
        printf("%ld, %f \t", ins_cnt, elapsed);

        user_delete(file, sort_query);
        printf("\n");
    }

    type_dtor(V_type);
    query_dtor(amount_query);
    query_dtor(sort_query);
    json_dtor(info_json);
    json_dtor(glossary);
    user_close_file(file);
}

void bench_read_inner()
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

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);
    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    File *file = user_open_file(test_file_name);

    TYPE_INIT(V_type, "V");
    user_add_type(file, V_type);

    List_Path *amount_q_key_path_list = list_Path_new();

    STR_INIT(flag_q_key, "flag");
    PATH(flag_q_key);
    // STR_INIT(city_q_key, "city");
    // PATH(city_q_key);
    // STR_INIT(amount_q_key, "amount");
    // PATH(amount_q_key);

    // list_Path_add(amount_q_key_path_list, city_q_key_path);
    // list_Path_add(amount_q_key_path_list, amount_q_key_path);
    list_Path_add(amount_q_key_path_list, flag_q_key_path);

    Query_item *amount_q_itm = query_item_new();
    // query_item_ctor(amount_q_itm, TYPE_INT32, amount_q_key_path_list);
    // amount_q_itm->query_val.int32_val = 50000;
    query_item_ctor(amount_q_itm, TYPE_BOOL, amount_q_key_path_list);
    amount_q_itm->query_val.bool_val = true;

    Query *query = query_new();
    query_item_add(query, amount_q_itm);

    for (size_t i = 1; i < 51; i++)
    {
        long ins_cnt = 0;
        for (size_t j = 0; j < 200 * i; j++)
        {
            if (user_write(file, info_json, V_type->name) == OK)
            {
                ins_cnt++;
            }
        }

        clock_t start = clock();

        struct Iter *iter = user_read(file, query);

        int cnt = 0;

        while (iter_is_avail(iter))
        {
            cnt++;

            iter_next(iter);
        }
        iter_dtor(iter);

        printf("Readed %d\t", cnt);

        clock_t end = clock();
        double elapsed = (double)(end - start) * 10 / CLOCKS_PER_SEC;

        printf("%ld, %f \t", ins_cnt, elapsed);

        printf("\n");

        user_delete(file, query);
    }

    type_dtor(V_type);
    query_dtor(query);
    json_dtor(info_json);
    user_close_file(file);
}