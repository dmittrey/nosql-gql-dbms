#include "network/driver.hpp"
#include "network/dbms_server.hpp"

extern "C"
{
    #include "tests/test.h"
}

int main(void)
{
    do_tests();

    Driver driver;
    DataBaseServiceImpl server{driver, 5433};

    return 0;
}