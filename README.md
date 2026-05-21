# NoSQL GQL Syntax DBMS

## DBMS Module

This module implements a three-layer architecture for a database management system. The path to the data file is configured through a CLI parameter. The server address and port are also provided via CLI arguments.

## GQL Client Module

This module implements a client that accepts queries written in GraphQL and sends them to the server at the address and port specified when starting the client.

## Protos Module

The `dbms.proto` file defines the message format and the service API configuration.

# Technologies

- Flex
- Bison
- gRPC
- Protobuf
- CMake
