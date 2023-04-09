#pragma once

#include <iostream> 
#include <string> 
#include <sstream> 

#include <grpcpp/grpcpp.h>

#include "dbms.pb.h"
#include "dbms.grpc.pb.h"

#include "network/request.hpp"
#include "network/response.hpp"

using dbms::DataBase;
using dbms::HelloRequest;
using dbms::HelloResponse;
using dbms::OperationRequest;
using dbms::OperationResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;

using Network::Json;
using Network::Request;

class DataBaseClient
{
private:
    std::unique_ptr<DataBase::Stub> stub_;

public:
    DataBaseClient(uint16_t port, std::string address = "localhost")
        : stub_(DataBase::NewStub(
              grpc::CreateChannel(address + ":" + std::to_string(port),
                                  grpc::InsecureChannelCredentials()))) {}

    bool Ping()
    {
        HelloRequest request;
        request.set_message("Ping");
        HelloResponse reply;
        ClientContext context;

        grpc::Status status = stub_->Ping(&context, request, &reply);

        if (status.ok())
        {
            std::cout << "Received from server: " << reply.message() << std::endl;
            return true;
        }
        else
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    void Apply(OperationRequest request)
    {
        ClientContext context;
        OperationResponse response;

        grpc::Status status = stub_->Apply(&context, request, &response);

        Network::Response operationResponse(response); 

        if (status.ok())
        {
            std::cout << response.DebugString() << std::endl;
            std::cout << "Status: OK" << std::endl;
            for (auto &i : operationResponse.collection)
            {
                std::cout << i << std::endl;
            }
        }
        else
        {
            std::cout << "OperationResponse rpc failed." << std::endl;
        }
    }
};