#pragma once

#include <vector>

extern "C"
{
#include "utils/utils.h"
#include "json/json.h"
}

namespace Network
{
    struct Response
    {
        Status status;
        std::vector<::Json> collection;
    };
}