#pragma once

#include <vector>

#include "network/request.hpp"
#include "network/status.hpp"

namespace Network
{
    struct Response
    {
        Status status;
        std::vector<Json> collection;
    };
}