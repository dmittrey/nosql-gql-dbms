#include "tests/test.h"

void bench_write();
void bench_read();
void bench_delete();
void bench_update();
void bench_read_inner();

int main(int argc, char **argv)
{
    do_tests();
    // bench_write();
    // bench_read();
    // bench_delete();
    bench_update();
    // bench_read_inner();
}