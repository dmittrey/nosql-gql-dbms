#include "json/pair.h"

PAIR_DEFINE(Json, Entity, json_dtor, json_cmp, entity_dtor, entity_cmp);
LIST_DEFINE(Pair_Json_Entity, pair_Json_Entity_dtor, pair_Json_Entity_cmp);