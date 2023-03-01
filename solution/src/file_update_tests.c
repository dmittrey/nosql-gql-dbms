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




void test_file_update()
{

}