cmake_minimum_required(VERSION 3.16)
project(dbms)

file(GLOB_RECURSE PROJECT_HEADERS include/*.h)
file(GLOB_RECURSE PROJECT_SOURCES src/*.c)

find_package(Boost 1.80 COMPONENTS serialization REQUIRED)

IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()

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

add_executable(${PROJECT_NAME} 
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
  ${dbms_grpc_srcs}
  ${dbms_proto_srcs}
  ${PROJECT_HEADERS} 
  ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    absl::flags
    absl::flags_parse
    ${_GRPC_GRPCPP_UNSECURE}
    ${_PROTOBUF_LIBPROTOBUF}
    ${_REFLECTION})
target_link_libraries(${PROJECT_NAME} Boost::serialization)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${_INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ./src)
target_include_directories(${PROJECT_NAME} PUBLIC ./include)