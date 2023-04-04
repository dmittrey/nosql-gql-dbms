#include <iostream>
#include <memory>
#include <string>

#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

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
using grpc::Status;

class DataBaseServiceImpl final : public DataBase::Service
{
private:
    std::unique_ptr<Server> server_;

    DataBaseServiceImpl() : DataBase::Service() {}

public:
    DataBaseServiceImpl(uint16_t port) : DataBaseServiceImpl("localhost", port) {}

    DataBaseServiceImpl(std::string address, uint16_t port)
    {
        ServerBuilder builder;

        DataBaseServiceImpl service{};

        std::string server_address = address + ":" + std::to_string(port);
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

        builder.RegisterService(&service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

        server->Wait();
    }

    Status Ping(ServerContext *context, const HelloRequest *request, HelloResponse *response) override
    {
        std::cout << "Received from client: " << request->message() << std::endl;
        response->set_message("Pong");
        return Status::OK;
    }

    Status Apply(ServerContext *context,
                 const OperationRequest *request,
                 ServerWriter<OperationResponse> *writer) override
    {
        OperationResponse response;
        response.set_xml(request->xml());

        // std::cout << response.xml().value() << std::endl;

        writer->Write(response);
        return Status::OK;
    }
};