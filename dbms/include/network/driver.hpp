#pragma once

#include <iostream>
#include <vector>

#include "request.hpp"
#include "response.hpp"

extern "C"
{
#include "user/user.h"
}

class Driver
{

public:
    File *file;

    Driver()
    {
        file = user_open_file("/Users/dmitry/Desktop/low-level-programming/dbms/driver.bin");
    }
    /**
     * Заходит запрос -> транслируем его в Command -> Обрабатываем на сервере
     */
    Network::Response execute(const Network::Request &request)
    {
        Iter *iter = NULL;

        TYPE_INIT(type, request.type_name_.c_str());
        user_add_type(file, type);

        Status apply_status = user_apply(file, request, &iter);

        std::vector<Network::Json> collection;
        while (iter_is_avail(iter))
        {
            std::cout << iter_get_json(iter)->key->val << std::endl;
            collection.push_back(*iter_get_json(iter));
            iter_next(iter);
        }

        return {.status = apply_status, .collection = collection};
    }
};