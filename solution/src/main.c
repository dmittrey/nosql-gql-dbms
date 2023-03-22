#include "tests/test.h"

void bench_write();
void bench_read();
void bench_delete();
void bench_update();

int main(int argc, char **argv)
{
    do_tests();
    // bench_write();
    // bench_read();
    // bench_delete();
    // bench_update();
}