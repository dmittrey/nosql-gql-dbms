#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
1) Обновить строку
2) Обновить инт
3) Обновить бул
4) Обновить флот
5) Обновить двух уровневый объект
6) Обновить трех уровневый объект
7) Обновить трех уровневый объект в разных секциях
8) Обновить объект на больший при отсутствии места для большего - должен записать в конец
9) Обновить обхект на больший при отсуствии места в секции - должен создать секцию и записать в конец
*/

static const char *test_file_name = "test.bin";

status_t File_UpdateType_Update(json_t *const json, json_t *const new_json)
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    ENTITY_INIT(entity, json, 0, 0, 0);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_update(file, wrt_addr, new_json));

    json_t * o_json = json_new();
    entity_t * o_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), entity, json));
    // Doesn't read json key!!!
    assert(json_cmp(o_json, new_json) == 0);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}


void test_file_update()
{
    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);

    File_UpdateType_Update(amount_json, location_json);
}