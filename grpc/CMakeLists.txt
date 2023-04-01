cmake_minimum_required(VERSION 3.8)

project(HelloWorld C CXX)

find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)

find_package(gRPC REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_REFLECTION gRPC::grpc++_reflection)

find_program(_PROTOBUF_PROTOC protoc)
message(STATUS "Using PROTOC ${_PROTOBUF_PROTOC}")

find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
message(STATUS "Using gRPC CPP Plugin ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

find_path(_INCLUDE google)
message(STATUS "Using include path ${_INCLUDE}")


# Proto file
get_filename_component(dbms_proto "../protos/dbms.proto" ABSOLUTE)
get_filename_component(dbms_proto_path "${dbms_proto}" PATH)

# Generated sources
set(dbms_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/dbms.pb.cc")
set(dbms_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/dbms.pb.h")
set(dbms_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/dbms.grpc.pb.cc")
set(dbms_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/dbms.grpc.pb.h")
add_custom_command(
      OUTPUT "${dbms_proto_srcs}" "${dbms_proto_hdrs}" "${dbms_grpc_srcs}" "${dbms_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${dbms_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${dbms_proto}"
      DEPENDS "${dbms_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${_INCLUDE}")
set (CMAKE_CXX_STANDARD 20)

# hw_grpc_proto
add_library(dbms_grpc_proto
  ${dbms_grpc_srcs}
  ${dbms_grpc_hdrs}
  ${dbms_proto_srcs}
  ${dbms_proto_hdrs})
target_link_libraries(dbms_grpc_proto
  ${_GRPC_GRPCPP_UNSECURE}
  ${_PROTOBUF_LIBPROTOBUF}
  ${_REFLECTION})

foreach(_target
  dbms_server dbms_client)
  add_executable(${_target} "${_target}.cc" "${dbms_grpc_srcs}" "${dbms_proto_srcs}")
  target_link_libraries(${_target}
    absl::flags
    absl::flags_parse
    ${_GRPC_GRPCPP_UNSECURE}
    ${_PROTOBUF_LIBPROTOBUF}
    ${_REFLECTION})
endforeach()

