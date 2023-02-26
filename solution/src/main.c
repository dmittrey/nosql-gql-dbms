void test_extents_write();
void test_extents_read();
void test_extents_update();
void test_header();

int main(int argc, char **argv)
{
    test_header();
    test_extents_write();
    test_extents_read();
    test_extents_update();
}