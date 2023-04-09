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

        Response(const dbms::OperationResponse &operationResponse)
        {
            status = static_cast<Status>(operationResponse.status());
            for (size_t i = 0; i < operationResponse.collection_size(); i++)
                collection.push_back(Json(operationResponse.collection(i)));
        }
    };
}