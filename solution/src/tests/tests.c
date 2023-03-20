#include "tests/test.h"

void do_tests()
{
    test_header();

    test_extents_write();
    test_extents_read();
    test_extents_delete();
    test_extents_update();

    test_extents_load();
    test_extents_find(); 

    test_types_write();
    test_types_delete();
    test_types_read();

    test_types_load();
    test_types_find();

    test_file_add_type();
    test_file_delete_type();

    test_file_write();
    // test_file_read();
    // test_file_delete();
    // test_file_update();

    // test_file_find();

    // test_iter();

    // bench_write();
    // bench_read();
    // bench_delete();
}