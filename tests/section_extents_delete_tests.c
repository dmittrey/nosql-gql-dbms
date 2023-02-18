#include <assert.h>
#include <string.h>

#include "utils.h"

static const char *test_file_name = "/Users/dmitry/Desktop/low-level-programming/test.bin";

static PerformStatus SectionExtents_DeleteStringJsonValue_Successful()
{
    
}

static PerformStatus SectionExtents_DeleteInt32JsonValue_Successful()
{

}

static PerformStatus SectionExtents_DeleteFloatJsonValue_Successful()
{

}

static PerformStatus SectionExtents_DeleteBoolJsonValue_Successful()
{

}

static PerformStatus SectionExtents_DeleteObjectJsonValue_Successful()
{

}

void test_extents_delete()
{
    assert(SectionExtents_DeleteStringJsonValue_Successful() == OK);
    assert(SectionExtents_DeleteInt32JsonValue_Successful() == OK);
    assert(SectionExtents_DeleteFloatJsonValue_Successful() == OK);
    assert(SectionExtents_DeleteBoolJsonValue_Successful() == OK);
    assert(SectionExtents_DeleteObjectJsonValue_Successful() == OK);
}