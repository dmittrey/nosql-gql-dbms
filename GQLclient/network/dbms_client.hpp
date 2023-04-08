#pragma once

#include <iostream> 
#include <string> 
#include <sstream> 

#include <grpcpp/grpcpp.h>

#include "dbms.pb.h"
#include "dbms.grpc.pb.h"

#include "network/request.hpp"

using dbms::DataBase;
using dbms::HelloRequest;
using dbms::HelloResponse;
using dbms::OperationRequest;
using dbms::OperationResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using Network::Json;
using Network::Request;

class DataBaseClient
{
private:
    std::unique_ptr<DataBase::Stub> stub_;

public:
    DataBaseClient(uint16_t port) : DataBaseClient("localhost", port) {}

    DataBaseClient(std::string address, uint16_t port)
        : stub_(DataBase::NewStub(
              grpc::CreateChannel(address + ":" + std::to_string(port),
                                  grpc::InsecureChannelCredentials()))) {}

    bool Ping()
    {
        HelloRequest request;
        request.set_message("Ping");
        HelloResponse reply;
        ClientContext context;

        Status status = stub_->Ping(&context, request, &reply);

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

        Status status = stub_->Apply(&context, request, &response);

        if (status.ok())
        {
            std::cout << "OperationResponse rpc succeeded." << std::endl;
        }
        else
        {
            std::cout << "OperationResponse rpc failed." << std::endl;
        }
    }
};