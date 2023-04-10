#include "network/driver.hpp"
#include "network/dbms_server.hpp"

extern "C"
{
#include "tests/test.h"
}

int main(int argc, char **argv)
{
    do_tests();

    Driver driver;
    DataBaseServiceImpl *server;
    if (argc == 1)
        server = new DataBaseServiceImpl(driver, 5432);
    if (argc == 2)
        server = new DataBaseServiceImpl(driver, std::stoi(argv[1]));
    else if (argc == 3)
        server = new DataBaseServiceImpl(driver, std::stoi(argv[1]), argv[2]);
    else
        return 1;

    return 0;
}