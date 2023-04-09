#pragma once

#include <vector>

#include "dbms.pb.h"
#include "dbms.grpc.pb.h"

extern "C"
{
#include "utils/utils.h"
#include "json/json.h"
}

using dbms::OperationResponse;

namespace Network
{
    struct Response
    {
        Status status;
        std::vector<Json> collection;

        operator OperationResponse() const
        {
            OperationResponse operationResponse;
            operationResponse.set_status(static_cast<dbms::Status>(status));
            for (auto &i : collection)
            {
                operationResponse.mutable_collection()->AddAllocated(i);
            }
            return operationResponse;
        }

        private:
            
    };
}