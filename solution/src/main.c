void test_header();

void test_extents_write();
void test_extents_read();
void test_extents_delete();
void test_extents_update();

void test_extents_load();
void test_extents_find();

void test_types_write();
void test_types_delete();
void test_types_read();

void test_types_load();
void test_types_find();

void test_file_add_type();

// void test_file_write();
// void test_file_read();
// void test_file_update();
// void test_file_delete();

// void test_file_find();

// void test_file_delete_type();

// void test_iter();

// void bench_write();
// void bench_read();
// void bench_delete();

int main(int argc, char **argv)
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

    // test_file_write();
    // test_file_read();
    // test_file_delete();
    // test_file_update();

    // test_file_find();

    // test_file_delete_type();

    // test_iter();

    // bench_write();
    // bench_read();
    // bench_delete();
}