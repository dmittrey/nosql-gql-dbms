#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "network/driver.hpp"

#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "dbms.pb.h"
#include "dbms.grpc.pb.h"

using dbms::DataBase;
using dbms::HelloRequest;
using dbms::HelloResponse;
using dbms::OperationRequest;
using dbms::OperationResponse;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;

using Network::Request;

class DataBaseServiceImpl final : public DataBase::Service
{
private:
    std::unique_ptr<Server> server_;
    Driver driver_;

    DataBaseServiceImpl(Driver &driver) : DataBase::Service(), driver_(driver) {}

public:
    DataBaseServiceImpl(Driver &driver, uint16_t port, std::string address = "localhost")
    {
        ServerBuilder builder;

        DataBaseServiceImpl service{driver};

        std::string server_address = address + ":" + std::to_string(port);
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

        builder.RegisterService(&service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

        server->Wait();
    }

    grpc::Status Ping(ServerContext *context, const HelloRequest *request, HelloResponse *response) override
    {
        std::cout << "Received from client: " << request->message() << std::endl;
        response->set_message("Pong");
        return grpc::Status::OK;
    }

    grpc::Status Apply(ServerContext *context,
                       const OperationRequest *req,
                       OperationResponse *resp) override
    {
        std::cout << req->DebugString() << std::endl;

        Request request(*req);
        *resp = driver_.execute(request);

        std::cout << resp->DebugString() << std::endl;

        return grpc::Status::OK;
    }
};