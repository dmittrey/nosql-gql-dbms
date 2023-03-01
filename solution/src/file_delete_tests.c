#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
Проверять:
Отсутствие ссылок к потомкам и братьям!!!

1) Удалить трехуровневую рутовую ноду
2) Удалить ноду второго уровня
3) Удалить ноду третьего уровня
4) Удалить строку
5) Удалить инт
6) Удалить бул
7) Удалить флот
8) Удалить ноду, находяющуюся в разных секциях
*/

static const char *test_file_name = "test.bin";




void test_file_delete()
{

}